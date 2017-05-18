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

	void checker::check_stmt(ysptr<stmt> st) {
		Match(st.get()) {
			Case(expr_stmt, Expression, Semicol, Return) {
				if (auto braced = dyn_cast<braced_expr>(Expression)) {
					// Braced statements are not return destinations
					braced->ReturnDest = false;
					check_expr(Expression);
				}
				else if (
					!Semicol
					&& !dyn_cast<const_asgn_expr>(Expression)) {
					// Not a constant assignment and does not end with semicolon
					Return = true;
					auto ret_ty = check_expr(Expression);
					auto ret_dest = m_Table.enclosing_return();
					if (!ret_dest) {
						throw std::exception("Sanity error: no enclosing return destination!");
					}
					if (auto o_ret_ty = ret_dest->ReturnType) {
						unify(ret_ty, o_ret_ty);
					}
					else {
						ret_dest->ReturnType = ret_ty;
						ret_dest->ReturnPos = st->Position;
					}
				}
				else {
					check_expr(Expression);
				}
			}
			Otherwise() {
				throw std::exception("Unhandled visit for semantic check (statement)!");
			}
		}
	}

	ysptr<type> checker::check_expr(ysptr<expr> ex) {
		Match(ex.get()) {
			Case(unit_expr) {
				return symbol_table::UNIT_T;
			}
			Case(ident_expr, Identifier) {
				ysptr<var_sym> sym = nullptr;
				bool hint_change = false;
				if (ex->HintType) {
					auto res = m_Table.ref(Identifier, ex->HintType);
					sym = res.first;
					hint_change = res.second;
				}
				else {
					sym = m_Table.ref(Identifier);
				}

				if (!sym) {
					if (hint_change) {
						rep::err_stream::report(
							rep::no_such_symbol(m_File,
								Identifier, ex->Position,
								"The hint type supported is wrong!",
								ex->HintPosition
							)
						);
					}
					else {
						rep::err_stream::report(
							rep::no_such_symbol(m_File,
								Identifier, ex->Position
							)
						);
					}
				}
				return sym->Type;
			}
			Case(int_lit_expr, Value) {
				return symbol_table::I32_T;
			}
			Case(real_lit_expr, Value) {
				return symbol_table::F32_T;
			}
			Case(preury_expr, Operator, Sub) {
				// TODO
			}
			Case(postury_expr, Operator, Sub) {
				// TODO
			}
			Case(binop_expr, Operator, LHS, RHS) {
				// TODO
			}
			Case(asgn_expr, LHS, RHS) {
				auto left = check_expr(LHS);
				auto right = check_expr(RHS);
				unify(left, right);
				return symbol_table::UNIT_T;
			}
			Case(const_asgn_expr, LHS, RHS) {
				auto left = dyn_cast<ident_expr>(LHS);
				auto right = check_expr(RHS);
				// TODO: Generalize function return type!
				auto sym_d = m_Table.ref(left->Identifier, right).first;
				if (sym_d) {
					std::string err = "TODO: cannot shadow constant assignment: " + left->Identifier;
					throw std::exception(err.c_str());
				}
				auto sym = std::make_shared
					<constant>(left->Identifier, right);
				m_Table.decl(sym);
				return symbol_table::UNIT_T;
			}
			Case(list_expr, Elements) {
				yvec<ysptr<type>> types;
				for (auto e : Elements) {
					types.push_back(check_expr(e));
				}
				return std::make_shared<type_cons>("Tuple", types);
			}
			Case(call_expr, Function, Args) {
				ysptr<type> args_ty = nullptr;
				if (Args) {
					args_ty = check_expr(Args);
				}
				else {
					args_ty = symbol_table::UNIT_T;
				}
				// Create hint type
				auto dummy_ty = fn_type_cons::create(args_ty, type_var::create());
				// Help call with filtering
				Function->HintType = dummy_ty;
				auto fn_ty = check_expr(Function);
				if (auto tt = dyn_cast<type_cons>(fn_ty)) {
					if (tt->Name != "Function") {
						throw std::exception("TODO: cannot call non-function expression!");
					}
					return *tt->Types.rbegin();
				}
				else {
					throw std::exception("TODO: cannot call non-function expression!");
				}
			}
			Case(block_expr, Statements, ReturnDest) {
				auto sc = m_Table.push(ReturnDest);
				for (auto st : Statements) {
					check_stmt(st);
				}
				m_Table.pop();
				if (auto ret_ty = sc->ReturnType) {
					return ret_ty;
				}
				else {
					return symbol_table::UNIT_T;
				}
			}
			Case(fnproto_expr, Parameters, ReturnType) {
				// TODO
			}
			Case(fn_expr, Parameters, ReturnType, Body) {
				// TODO: warn if no parameter name
				// TODO: error if same parameter name
				auto sc = m_Table.push(true);

				// Parameters ////////////////////////////////////////
				yvec<ysptr<type>> params;
				for (auto par : Parameters) {
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
				if (ReturnType) {
					ret_t = check_type(ReturnType);
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

				auto body_t = check_expr(Body);
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
			}
			Case(let_expr, Pattern, Type, Value) {
				ysptr<type> fin_sym = nullptr;
				ysptr<type> ty_sym = nullptr;
				ysptr<type> val_sym = nullptr;
				if (Type) {
					ty_sym = check_type(Type);
					if (Value) {
						Value->HintType = ty_sym;
						Value->HintPosition = Type->Position;
					}
					fin_sym = ty_sym;
				}
				if (Value) {
					val_sym = check_expr(Value);
					fin_sym = val_sym;
				}
				if (ty_sym && val_sym) {
					unify(ty_sym, val_sym);
				}
				auto entries = match_pat(Pattern, fin_sym);
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
			Otherwise() {
				throw std::exception("Unhandled visit for semantic check (expression)!");
			}
		}
		// TODO: assert
		return nullptr;
	}

	ysptr<type> checker::check_type(ysptr<ty_expr> ty) {
		Match(ty.get()) {
			Case(unit_ty_expr) {
				return symbol_table::UNIT_T;
			}
			Case(ident_ty_expr, Identifier) {
				// TODO: real type lookup
				if (Identifier == "i32") return symbol_table::I32_T;
				if (Identifier == "f32") return symbol_table::F32_T;
				return nullptr;
			}
			Case(bin_ty_expr, Operator, LHS, RHS) {
				if (Operator.Type == ytoken_t::Arrow) {
					auto left = check_type(LHS);
					auto right = check_type(RHS);
					return fn_type_cons::create(left, right);
				}
				else {
					throw std::exception("TODO: no such type operator");
				}
			}
			Case(list_ty_expr, Elements) {
				yvec<ysptr<type>> syms;
				for (auto& el : Elements) {
					syms.push_back(check_type(el));
				}
				return std::make_shared<type_cons>("Tuple", syms);
			}
			Otherwise() {
				throw std::exception("Unhandled visit for semantic check (type)!");
			}
		}
		// TODO: assert
		return nullptr;
	}

	ysptr<type> checker::prune(ysptr<type> t) {
		Match(t.get()) {
			Case(type_var, Instance) {
				if (Instance) {
					Instance = prune(Instance);
					return Instance;
				}
				return t;
			}
			Otherwise() {
				return t;
			}
		}
		// TODO: assert
		return nullptr;
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
					unify(tt1->Types[i], tt1->Types[i]);
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

	yvec<ypair<ystr, ysptr<type>>> checker::match_pat(ysptr<pat_expr> pat, ysptr<type> ty) {
		yvec<ypair<ystr, ysptr<type>>> res;
		match_pat_impl(res, pat, ty);
		return res;
	}

	void checker::match_pat_impl(yvec<ypair<ystr, ysptr<type>>>& res, ysptr<pat_expr> pat, ysptr<type> ty) {
		Match(pat.get()) {
			Case(ignore_pat_expr) {
				return;
			}
			Case(unit_pat_expr) {
				if (!ty) {
					throw std::exception("TODO: Unmeaningful bind without type!");
				}
				if (ty != symbol_table::UNIT_T) {
					throw std::exception("TODO: Pattern match failed for unit! (type)");
				}
				return;
			}
			Case(ident_pat_expr, Identifier) {
				// Just bind
				if (ty) {
					res.push_back({ Identifier, ty });
				}
				else {
					res.push_back({ Identifier, nullptr });
				}
				return;
			}
			Case(list_pat_expr, Elements) {
				if (ty) {
					if (auto tt = dyn_cast<type_cons>(ty)) {
						if (tt->Name != "Tuple") {
							throw std::exception("TODO: Cannot match non-tuple with tuple");
						}
						if (tt->Types.size() != Elements.size()) {
							throw std::exception("TODO: Pattern list size mismatch!");
						}

						for (ysize i = 0; i < Elements.size(); i++) {
							match_pat_impl(res, Elements[i], tt->Types[i]);
						}
					}
					else {
						throw std::exception("TODO: Cannot match non-tuple with tuple");
					}
				}
				else {
					for (ysize i = 0; i < Elements.size(); i++) {
						match_pat_impl(res, Elements[i], nullptr);
					}
				}
				return;
			}
			Otherwise() {
				throw std::exception("Unhandled visit for pattern matching (expression)!");
			}
		}
	}
}
