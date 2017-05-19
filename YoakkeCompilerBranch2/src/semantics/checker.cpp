#include "checker.h"
#include "../utility/match.h"
#include "../ast/ast.h"
#include "type.h"
#include "var_sym.h"
#include "../reporting/err_stream.h"
#include "../reporting/err_msg.h"
#include "scope.h"

namespace yk {
	checker::checker(file_handle const& f)
		: m_File(f) {
	}

	void checker::check_stmt(stmt const& st) {
		match(st.Data) (
			[this, &st](ysptr<expr_stmt> exp) {
				auto& sub = exp->get<0>();
				auto& semicol = exp->get<1>();
				auto& do_ret = exp->get<2>();
				if (bool* ret_dst = get_braced(*sub)) {
					// Braced statements are not return destinations
					*ret_dst = false;
					check_expr(*sub);
				}
				else if (
					!semicol
					&& !std::get_if<ysptr<const_asgn_expr>>(&sub->Data)) {
					// Not a constant assignment and does not end with semicolon
					do_ret = true;
					auto ret_ty = check_expr(*sub);
					auto ret_dest = m_Table.enclosing_return();
					if (!ret_dest) {
						throw std::exception("Sanity error: no enclosing return destination!");
					}
					if (auto o_ret_ty = ret_dest->ReturnType) {
						unify(ret_ty, o_ret_ty);
					}
					else {
						ret_dest->ReturnType = ret_ty;
						ret_dest->ReturnPos = st.Position;
					}
				}
				else {
					check_expr(*sub);
				}
			}
		);
	}

	ysptr<type> checker::check_expr(expr const& ex) {
		return match(ex.Data) (
			[](ysptr<unit_expr>) -> ysptr<type> {
				return symbol_table::UNIT_T;
			},
			[this, &ex](ysptr<ident_expr> ie) -> ysptr<type> {
				auto& ident = ie->get<0>();
				ysptr<var_sym> sym = nullptr;
				bool hint_change = false;
				if (ex.HintType) {
					std::tie(sym, hint_change) = m_Table.ref(ident, ex.HintType);
				}
				else {
					sym = m_Table.ref(ident);
				}

				if (!sym) {
					if (hint_change) {
						rep::err_stream::report(
							rep::no_such_symbol(m_File,
								ident, ex.Position,
								"The hint type supported is wrong!",
								ex.HintPosition
							)
						);
					}
					else {
						rep::err_stream::report(
							rep::no_such_symbol(m_File,
								ident, ex.Position
							)
						);
					}
				}
				return sym->Type;
			},
			[](ysptr<int_lit_expr>) -> ysptr<type> {
				return symbol_table::I32_T;
			},
			[](ysptr<real_lit_expr>) -> ysptr<type> {
				return symbol_table::F32_T;
			},
			[](ysptr<preury_expr>) -> ysptr<type> {
				// TODO
				return nullptr;
			},
			[](ysptr<postury_expr>) -> ysptr<type> {
				// TODO
				return nullptr;
			},
			[](ysptr<binop_expr>) -> ysptr<type> {
				// TODO
				return nullptr;
			},
			[this](ysptr<asgn_expr> ae) -> ysptr<type> {
				auto left = check_expr(ae->get<1>());
				auto right = check_expr(ae->get<2>());
				unify(left, right);
				return symbol_table::UNIT_T;
			},
			[this](ysptr<const_asgn_expr> ce) -> ysptr<type> {
				auto& left = std::get<ysptr<ident_expr>>(ce->get<1>().Data);
				auto const& right = check_expr(ce->get<2>());
				auto& ident = left->get<0>();

				auto sym_d = m_Table.ref(ident, right).first;
				if (sym_d) {
					std::string err = "TODO: cannot shadow constant assignment: " + ident;
					throw std::exception(err.c_str());
				}
				auto sym = std::make_shared
					<constant>(ident, right);
				m_Table.decl(sym);
				return symbol_table::UNIT_T;
			},
			[this](ysptr<list_expr> le) -> ysptr<type> {
				auto& elements = le->get<0>();
				yvec<ysptr<type>> types;
				for (auto e : elements) {
					types.push_back(check_expr(e));
				}
				return std::make_shared<type_cons>("Tuple", types);
			},
			[this](ysptr<call_expr> ce) -> ysptr<type> {
				ysptr<type> args_ty = nullptr;
				auto& function = ce->get<0>();
				auto& args = ce->get<1>();
				if (args) {
					args_ty = check_expr(*args);
				}
				else {
					args_ty = symbol_table::UNIT_T;
				}
				// Create hint type
				auto dummy_ty = fn_type_cons::create(args_ty, type_var::create());
				// Help call with filtering
				function.HintType = dummy_ty;
				auto fn_ty = check_expr(function);
				if (auto tt = dyn_cast<type_cons>(fn_ty)) {
					if (tt->Name != "Function") {
						throw std::exception("TODO: cannot call non-function expression!");
					}
					return *tt->Types.rbegin();
				}
				else {
					throw std::exception("TODO: cannot call non-function expression!");
				}
			},
			[this](ysptr<block_expr> be) -> ysptr<type> {
				auto& statements = be->get<0>();
				auto& ret_dest = be->get<1>();
				auto sc = m_Table.push(ret_dest);
				for (auto st : statements) {
					check_stmt(st);
				}
				m_Table.pop();
				if (auto ret_ty = sc->ReturnType) {
					return ret_ty;
				}
				else {
					return symbol_table::UNIT_T;
				}
			},
			[](ysptr<fnproto_expr>) -> ysptr<type> {
				// TODO
				return nullptr;
			},
			[this](ysptr<fn_expr> fe) -> ysptr<type> {
				auto& parameters = fe->get<0>();
				auto& returntype = fe->get<1>();
				auto& body = fe->get<2>();
				// TODO: warn if no parameter name
				// TODO: error if same parameter name
				auto sc = m_Table.push(true);

				// Parameters ////////////////////////////////////////
				yvec<ysptr<type>> params;
				for (auto par : parameters) {
					auto par_ty = check_type(par.second);
					params.push_back(par_ty);
					if (auto name = par.first) {
						auto par_sym = std::make_shared
							<variable>(name.value().Value, par_ty);
						m_Table.decl(par_sym);
					}
				}
				//////////////////////////////////////////////////////

				// Return type
				ysptr<type> ret_t = nullptr;
				if (returntype) {
					ret_t = check_type(*returntype);
				}
				else {
					ret_t = symbol_table::UNIT_T;
				}

				// Create the type
				ysptr<type> fn_ty;
				if (params.empty()) {
					fn_ty = fn_type_cons::create(symbol_table::UNIT_T, ret_t);
				}
				else if (params.size() > 1) {
					ysptr<type> par_tpl =
						std::make_shared<type_cons>("Tuple", params);
					fn_ty = fn_type_cons::create(par_tpl, ret_t);
				}
				else {
					fn_ty = fn_type_cons::create(params[0], ret_t);
				}

				auto body_t = check_expr(body);
				m_Table.pop();

				// Check if body returned what return type needs
				if (ret_t && body_t) {
					unify(ret_t, body_t);
				}
				else if (ret_t) {
					throw std::exception("TODO: function return type mismatch! (body no return)");
				}
				else if (body_t) {
					throw std::exception("TODO: function return type mismatch! (no annotation)");
				}

				return fn_ty;
			},
			[this](ysptr<let_expr> le) -> ysptr<type> {
				auto& pattern = le->get<0>();
				auto& ttype = le->get<1>();
				auto& value = le->get<2>();
				ysptr<type> fin_sym = nullptr;
				ysptr<type> ty_sym = nullptr;
				ysptr<type> val_sym = nullptr;
				if (ttype) {
					ty_sym = check_type(*ttype);
					if (value) {
						value->HintType = ty_sym;
						value->HintPosition = ttype->Position;
					}
					fin_sym = ty_sym;
				}
				if (value) {
					val_sym = check_expr(*value);
					fin_sym = val_sym;
				}
				if (ty_sym && val_sym) {
					unify(ty_sym, val_sym);
				}
				auto entries = match_pat(pattern, fin_sym);
				for (auto e : entries) {
					if (e.second) {
						auto var_sym = std::make_shared
							<variable>(e.first, e.second);
						m_Table.decl(var_sym);
					}
					else {
						auto var_sym = std::make_shared
							<variable>(e.first, type_var::create());
						m_Table.decl(var_sym);
					}
				}
				return symbol_table::UNIT_T;
			}
		);
	}

	ysptr<type> checker::check_type(ty_expr const& ty) {
		return match(ty.Data) (
			[](ysptr<unit_ty_expr>) -> ysptr<type> {
				return symbol_table::UNIT_T;
			},
			[](ysptr<ident_ty_expr> ie) -> ysptr<type> {
				auto& ident = ie->get<0>();
				// TODO: real type lookup
				if (ident == "i32") return symbol_table::I32_T;
				if (ident == "f32") return symbol_table::F32_T;
				return nullptr;
			},
			[this](ysptr<bin_ty_expr> be) -> ysptr<type> {
				auto& oper = be->get<0>();
				if (oper.Type == ytoken_t::Arrow) {
					auto left = check_type(be->get<1>());
					auto right = check_type(be->get<2>());
					return fn_type_cons::create(left, right);
				}
				else {
					throw std::exception("TODO: no such type operator");
				}
			},
			[this](ysptr<list_ty_expr> le) -> ysptr<type> {
				auto& elements = le->get<0>();
				yvec<ysptr<type>> syms;
				for (auto& el : elements) {
					syms.push_back(check_type(el));
				}
				return std::make_shared<type_cons>("Tuple", syms);
			}
		);
	}

	ysptr<type> checker::prune(ysptr<type> t) {
		if (auto tv = dyn_cast<type_var>(t)) {
			if (tv->Instance) {
				tv->Instance = prune(tv->Instance);
				return tv->Instance;
			}
			return t;
		}
		else {
			return t;
		}
	}
	
	void checker::unify(ysptr<type> t1, ysptr<type> t2) {
		t1 = prune(t1);
		t2 = prune(t2);
		if (t1 == t2) {
			return;
		}

		if (auto tt1 = dyn_cast<type_cons>(t1)) {
			if (auto tt2 = dyn_cast<type_cons>(t2)) {
				if (tt1->Name != tt2->Name) {
					// TODO
					throw std::exception(("TODO: " + t1->to_str() + " is not " + t2->to_str()).c_str());
				}
				if (tt1->Types.size() != tt2->Types.size()) {
					// TODO
					throw std::exception(("TODO: " + t1->to_str() + " has not as many types as " + t2->to_str()).c_str());
				}
				for (ysize i = 0; i < tt1->Types.size(); i++) {
					unify(tt1->Types[i], tt2->Types[i]);
				}
			}
			else if (auto tt2 = dyn_cast<type_var>(t2)) {
				if (t1->contains(dyn_cast<type_var>(t2))) {
					// TODO
					throw std::exception(("TODO: " + t1->to_str() + " contains " + t2->to_str() + " so it's recursive").c_str());
				}
				else {
					tt2->Instance = t1;
				}
			}
			else {
				throw std::exception("Unify uncovered case!");
			}
		}
		else if (auto tt1 = dyn_cast<type_var>(t1)) {
			if (auto tt2 = dyn_cast<type_cons>(t2)) {
				if (tt2->contains(tt1)) {
					// TODO
					throw std::exception(("TODO: " + t2->to_str() + " contains " + t1->to_str() + " so it's recursive").c_str());
				}
				else {
					tt1->Instance = tt2;
				}
			}
			else if (auto tt2 = dyn_cast<type_var>(t2)) {
				tt2->Instance = tt1;
			}
			else {
				throw std::exception("Unify uncovered case!");
			}
		}
		else {
			throw std::exception("Unify uncovered case!");
		}
	}

	bool* checker::get_braced(expr const& e) {
		if (auto bl = std::get_if<ysptr<block_expr>>(&e.Data)) {
			return &(*bl)->get<1>();
		}
		return nullptr;
	}

	yvec<ypair<ystr, ysptr<type>>> checker::match_pat(pat_expr const& pat, ysptr<type> ty) {
		yvec<ypair<ystr, ysptr<type>>> res;
		match_pat_impl(res, pat, ty);
		return res;
	}

	void checker::match_pat_impl(yvec<ypair<ystr, ysptr<type>>>& res, pat_expr const& pat, ysptr<type> ty) {
		match(pat.Data) (
			[](ysptr<ignore_pat_expr>) {
				return;
			},
			[&ty](ysptr<unit_pat_expr> ) {
				if (!ty) {
					throw std::exception("TODO: Unmeaningful bind without type!");
				}
				if (ty != symbol_table::UNIT_T) {
					throw std::exception("TODO: Pattern match failed for unit! (type)");
				}
				return;
			},
			[&ty, &res](ysptr<ident_pat_expr> ip) {
				auto& ident = ip->get<0>();
				// Just bind
				if (ty) {
					res.push_back({ ident, ty });
				}
				else {
					res.push_back({ ident, nullptr });
				}
				return;
			},
			[&ty, &res, this](ysptr<list_pat_expr> le) {
				auto& elements = le->get<0>();
				if (ty) {
					if (auto tt = dyn_cast<type_cons>(ty)) {
						if (tt->Name != "Tuple") {
							throw std::exception("TODO: Cannot match non-tuple with tuple");
						}
						if (tt->Types.size() != elements.size()) {
							throw std::exception("TODO: Pattern list size mismatch!");
						}

						for (ysize i = 0; i < elements.size(); i++) {
							match_pat_impl(res, elements[i], tt->Types[i]);
						}
					}
					else {
						throw std::exception("TODO: Cannot match non-tuple with tuple");
					}
				}
				else {
					for (ysize i = 0; i < elements.size(); i++) {
						match_pat_impl(res, elements[i], nullptr);
					}
				}
			}
		);
	}
}
