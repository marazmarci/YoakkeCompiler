#include "checker.h"
#include "../utility/match.h"
#include "../ast/ast.h"
#include "type.h"
#include "var_sym.h"
#include "../reporting/err_stream.h"
#include "../reporting/err_msg.h"

namespace yk {
	checker::checker(file_handle const& f)
		: m_File(f) {
	}

	void checker::check_stmt(ysptr<stmt> st) {
		Match(st.get()) {
			Case(expr_stmt, Expression, Semicol, Return) {
				check_expr(Expression);
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
				// TODO: Hint change?
				if (ex->HintType) {
					sym = m_Table.ref(Identifier, ex->HintType);
				}
				else {
					sym = m_Table.ref(Identifier);
				}

				if (!sym) {
					rep::err_stream::report(
						rep::no_such_symbol(m_File,
							Identifier, ex->Position
						)
					);
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
				auto left = std::dynamic_pointer_cast<ident_expr>(LHS);
				auto right = check_expr(RHS);
				auto sym_d = m_Table.ref(left->Identifier, right);
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
				auto dummy_ty = type_cons::create
					("Function", args_ty, type_var::create());
				// Help call with filtering
				Function->HintType = dummy_ty;
				auto fn_ty = check_expr(Function);
				if (auto tt = std::dynamic_pointer_cast<type_cons>(fn_ty)) {
					if (tt->Name != "Function") {
						throw std::exception("TODO: cannot call non-function expression!");
					}
					return *tt->Types.rbegin();
				}
				else {
					throw std::exception("TODO: cannot call non-function expression!");
				}
			}
			Case(block_expr, Statements, ReturnDestination, Scope) {
				// TODO: return type, dest
				m_Table.push();
				for (auto st : Statements) {
					check_stmt(st);
				}
				m_Table.pop();
				return symbol_table::UNIT_T;
			}
			Case(fnproto_expr, Parameters, ReturnType) {
				// TODO
			}
			Case(fn_expr, Parameters, ReturnType, Body) {
				// TODO: warn if no parameter name
				// TODO: Functions stop returning anyways, mark it!
				m_Table.push();

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
				ysptr<type> ret = nullptr;
				if (ReturnType) {
					ret = check_type(ReturnType);
				}
				else {
					ret = symbol_table::UNIT_T;
				}

				// Create the type
				ysptr<type> fn_ty;
				if (params.empty()) {
					fn_ty = type_cons::create
						("Function", symbol_table::UNIT_T, ret);
				}
				else if (params.size() > 1) {
					ysptr<type> par_tpl =
						std::make_shared<type_cons>("Tuple", params);
					fn_ty = type_cons::create
						("Function", par_tpl, ret);
				}
				else {
					fn_ty = type_cons::create
						("Function", params[0], ret);
				}

				auto body_t = check_expr(Body);
				m_Table.pop();

				// Check if body returned what return type needs
				/*
				if (!ret->same(body_t)) {
					if (ReturnType) {
						if (Body->Scope->get_return_type()) {
							// TODO: Add message
							rep::err_stream::report(
								rep::type_mismatch(
									m_File,
									ReturnType->Position,
									Body->Scope->get_return_type_pos()
								)
							);
						}
						else {
							throw std::exception("TODO: function return type mismatch! (body no return)");
						}
					}
					else {
						throw std::exception("TODO: function return type mismatch! (nopos)");
					}
				}
				*/

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
					return type_cons::create("Function", left, right);
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
	}
	
	void checker::unify(ysptr<type> t1, ysptr<type> t2) {
		t1 = prune(t1);
		t2 = prune(t2);
		if (t1 == t2) {
			return;
		}

		if (auto tt1 = std::dynamic_pointer_cast<type_cons>(t1)) {
			if (auto tt2 = std::dynamic_pointer_cast<type_cons>(t2)) {
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
			else if (auto tt2 = std::dynamic_pointer_cast<type_var>(t2)) {
				if (t1->contains(std::dynamic_pointer_cast<type_var>(t2))) {
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
		else if (auto tt1 = std::dynamic_pointer_cast<type_var>(t1)) {
			if (auto tt2 = std::dynamic_pointer_cast<type_cons>(t2)) {
				if (tt2->contains(tt1)) {
					// TODO
					throw std::exception(("TODO: " + t2->to_str() + " contains " + t1->to_str() + " so it's recursive").c_str());
				}
				else {
					tt1->Instance = tt2;
				}
			}
			else if (auto tt2 = std::dynamic_pointer_cast<type_var>(t2)) {
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
					if (auto tt = std::dynamic_pointer_cast<type_cons>(ty)) {
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
