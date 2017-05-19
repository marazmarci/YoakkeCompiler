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

	void checker::check_stmt(stmt& st) {
		match(st.Data) (
			[&](ysptr<expr_stmt> exp) {
				auto& sub		= exp->get<0>();
				auto& semicol	= exp->get<1>();
				auto& do_ret	= exp->get<2>();

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
						unify(ret_ty, *o_ret_ty);
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

	type checker::check_expr(expr& ex) {
		return match(ex.Data) (
			[&](ysptr<unit_expr>) -> type {
				// TODO
				//return symbol_table::UNIT_T;
				return type::create_cons("unit");
			},
			[&](ysptr<ident_expr> ie) -> type {
				ystr& ident = ie->get<0>();

				ysptr<var_sym> sym = nullptr;
				bool hint_change = false;
				if (ex.HintType) {
					std::tie(sym, hint_change) = m_Table.ref(ident, *ex.HintType);
					//sym = res.first;
					//hint_change = res.second;
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
			[&](ysptr<int_lit_expr>) -> type {
				return symbol_table::I32_T;
			},
			[&](ysptr<real_lit_expr>) -> type {
				return symbol_table::F32_T;
			},
			[&](ysptr<preury_expr>) -> type {
				// TODO
				return symbol_table::UNIT_T;
			},
			[&](ysptr<postury_expr>) -> type {
				// TODO
				return symbol_table::UNIT_T;
			},
			[&](ysptr<binop_expr>) -> type {
				// TODO
				return symbol_table::UNIT_T;
			},
			[&](ysptr<asgn_expr> ae) -> type {
				// Don't need get<0> because it's the operator
				auto left	= check_expr(ae->get<1>());
				auto right	= check_expr(ae->get<2>());
				unify(left, right);
				return symbol_table::UNIT_T;
			},
			[&](ysptr<const_asgn_expr> ce) -> type {
				// Don't need get<0> because it's the operator
				auto& left = std::get<ysptr<ident_expr>>(ce->get<1>().Data);
				auto& ident = left->get<0>();
				auto right = check_expr(ce->get<2>());

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
			[&](ysptr<list_expr> le) -> type {
				auto& elements = le->get<0>();

				yvec<type> types;
				for (auto e : elements) {
					types.push_back(check_expr(e));
				}
				return type::create_tup(types);
			},
			[&](ysptr<call_expr> ce) -> type {
				auto& function	= ce->get<0>();
				auto& args		= ce->get<1>();

				type args_ty = args ?
					  check_expr(*args)
					: symbol_table::UNIT_T;

				// Create hint type
				auto dummy_ty = type::create_fn(args_ty, type::create_var());
				// Help call with filtering
				function.HintType = dummy_ty;
				auto fn_ty = check_expr(function);
				return match(fn_ty.Data) (
					[&](ysptr<cons_type> t) -> type {
						if (t->get<0>() != "@fn") {
							throw std::exception("TODO: cannot call non-function expression!");
						}
						return *t->get<1>().rbegin();
					},
					[&](auto&) -> type {
						throw std::exception("TODO: cannot call non-function expression!");
					}
				);
			},
			[&](ysptr<block_expr> be) -> type {
				auto& statements	= be->get<0>();
				auto& ret_dest		= be->get<1>();

				auto sc = m_Table.push(ret_dest);
				for (auto st : statements) {
					check_stmt(st);
				}
				m_Table.pop();
				if (auto ret_ty = sc->ReturnType) {
					return *ret_ty;
				}
				else {
					return symbol_table::UNIT_T;
				}
			},
			[&](ysptr<fnproto_expr>) -> type {
				// TODO
				return symbol_table::UNIT_T;
			},
			[&](ysptr<fn_expr> fe) -> type {
				auto& parameters	= fe->get<0>();
				auto& returntype	= fe->get<1>();
				auto& body			= fe->get<2>();

				// TODO: warn if no parameter name
				// TODO: error if same parameter name
				auto sc = m_Table.push(true);

				// Parameters ////////////////////////////////////////
				yvec<type> params;
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
				type ret_t = returntype ? 
					  check_type(*returntype)
					: symbol_table::UNIT_T;

				// Create the type
				type fn_ty;
				if (params.empty()) {
					fn_ty = type::create_fn(symbol_table::UNIT_T, ret_t);
				}
				else if (params.size() > 1) {
					type par_tpl = type::create_tup(params);
					fn_ty = type::create_fn(par_tpl, ret_t);
				}
				else {
					fn_ty = type::create_fn(params[0], ret_t);
				}

				auto body_t = check_expr(body);
				m_Table.pop();

				// Check if body returned what return type needs
				unify(ret_t, body_t);
				return fn_ty;
			},
			[&](ysptr<let_expr> le) -> type {
				auto& pattern	= le->get<0>();
				auto& ttype		= le->get<1>();
				auto& value		= le->get<2>();

				yopt<type> fin_sym = {};
				yopt<type> ty_sym = {};
				yopt<type> val_sym = {};
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
					unify(*ty_sym, *val_sym);
				}
				auto entries = match_pat(pattern, fin_sym);
				for (auto e : entries) {
					if (e.second) {
						auto var_sym = std::make_shared
							<variable>(e.first, *e.second);
						m_Table.decl(var_sym);
					}
					else {
						auto var_sym = std::make_shared
							<variable>(e.first, type::create_var());
						m_Table.decl(var_sym);
					}
				}
				return symbol_table::UNIT_T;
			}
		);
	}

	type checker::check_type(ty_expr& ty) {
		return match(ty.Data) (
			[&](ysptr<unit_ty_expr>) -> type {
				return symbol_table::UNIT_T;
			},
			[&](ysptr<ident_ty_expr> ie) -> type {
				auto& ident = ie->get<0>();

				// TODO: real type lookup
				if (ident == "i32") return symbol_table::I32_T;
				if (ident == "f32") return symbol_table::F32_T;
				return symbol_table::UNIT_T;
			},
			[&](ysptr<bin_ty_expr> be) -> type {
				auto& oper = be->get<0>();

				if (oper.Type == ytoken_t::Arrow) {
					auto left = check_type(be->get<1>());
					auto right = check_type(be->get<2>());
					return type::create_fn(left, right);
				}
				else {
					throw std::exception("TODO: no such type operator");
				}
			},
			[&](ysptr<list_ty_expr> le) -> type {
				auto& elements = le->get<0>();

				yvec<type> syms;
				for (auto& el : elements) {
					syms.push_back(check_type(el));
				}
				return type::create_tup(syms);
			}
		);
	}
	
	void checker::unify(type& t1, type& t2) {
		t1 = t1.prune();
		t2 = t2.prune();
		if (t1.same(t2)) {
			return;
		}

		match(t1.Data, t2.Data) (
			[&](ysptr<cons_type> tt1, ysptr<cons_type> tt2) {
				auto& name1 = tt1->get<0>();
				auto& name2 = tt2->get<0>();
				auto& types1 = tt1->get<1>();
				auto& types2 = tt2->get<1>();

				if (name1 != name2) {
					// TODO
					throw std::exception(("TODO: " + t1.to_str() + " is not " + t2.to_str()).c_str());
				}
				if (types1.size() != types2.size()) {
					// TODO
					throw std::exception(("TODO: " + t1.to_str() + " has not as many types as " + t2.to_str()).c_str());
				}
				for (ysize i = 0; i < types1.size(); i++) {
					unify(types1[i], types2[i]);
				}
			},
			[&](ysptr<var_type> tt1, ysptr<var_type> tt2) {
				// Substitute
				tt2->get<1>() = tt1;
			},
			[&](ysptr<cons_type> tt1, ysptr<var_type> tt2) {
				if (t1.contains(tt2)) {
					// TODO
					throw std::exception(("TODO: " + t1.to_str() + " contains " + t2.to_str() + " so it's recursive").c_str());
				}
				else {
					tt2->get<1>() = t1;
				}
			},
			[&](ysptr<var_type> tt1, ysptr<cons_type> tt2) {
				if (t2.contains(tt1)) {
					// TODO
					throw std::exception(("TODO: " + t2.to_str() + " contains " + t1.to_str() + " so it's recursive").c_str());
				}
				else {
					tt1->get<1>() = t2;
				}
			}
		);
	}

	bool* checker::get_braced(expr const& e) {
		return match(e.Data) (
			[&](ysptr<block_expr> be) -> bool* { return &be->get<1>(); },
			[&](auto&) -> bool* { return nullptr; }
		);
	}

	yvec<ypair<ystr, yopt<type>>> checker::match_pat(pat_expr const& pat, yopt<type> ty) {
		yvec<ypair<ystr, yopt<type>>> res;
		match_pat_impl(res, pat, ty);
		return res;
	}

	void checker::match_pat_impl(yvec<ypair<ystr, yopt<type>>>& res, pat_expr const& pat, yopt<type> ty) {
		match(pat.Data) (
			[&](ysptr<ignore_pat_expr>) {
				return;
			},
			[&](ysptr<unit_pat_expr> ) {
				if (!ty) {
					throw std::exception("TODO: Unmeaningful bind without type!");
				}
				if (ty->same(symbol_table::UNIT_T)) {
					throw std::exception("TODO: Pattern match failed for unit! (type)");
				}
				return;
			},
			[&](ysptr<ident_pat_expr> ip) {
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
			[&](ysptr<list_pat_expr> le) {
				auto& elements = le->get<0>();
				if (ty) {
					match(ty->Data) (
						[&](ysptr<cons_type> tt) {
							auto& name = tt->get<0>();
							auto& types = tt->get<1>();
							if (name != "@tup") {
								throw std::exception("TODO: Cannot match non-tuple with tuple");
							}
							if (types.size() != elements.size()) {
								throw std::exception("TODO: Pattern list size mismatch!");
							}

							for (ysize i = 0; i < elements.size(); i++) {
								match_pat_impl(res, elements[i], types[i]);
							}
						},
						[&](auto&) { 
							throw std::exception("TODO: Cannot match non-tuple with tuple"); 
						}
					);
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
