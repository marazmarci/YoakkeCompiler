#include "semantic_checker.h"
#include "type_symbol.h"
#include "typed_symbol.h"
#include "../lexing/ytoken_t.h"
#include "../ast/stmt.h"
#include "../ast/expr.h"
#include "../ast/type_desc.h"
#include "../ast/pattern.h"
#include "../utility/match.h"
#include "../reporting/err_stream.h"
#include "../reporting/err_msg.h"

namespace yk {
	semantic_checker::semantic_checker(file_handle const& f)
		: m_File(f) {
	}

	void semantic_checker::check_stmt(ysptr<stmt> st) {
		Match(st.get()) {
			Case(expr_stmt, Expression, Semicol, Return) {
				if (auto braced =
					std::dynamic_pointer_cast<braced_expr>(Expression)) {
					braced->ReturnDestination = false;
					check_expr(Expression);
				}
				else if (!Semicol
					&& !std::dynamic_pointer_cast<const_asgn_expr>(Expression)) {
					Return = true;
					auto rett = check_expr(Expression);
					auto ret_dest = m_Table.get_enclosing_return_dest();
					if (!ret_dest) {
						throw std::exception("Sanity error: no enclosing return destination!");
					}
					if (auto exp_ty = ret_dest->get_return_type()) {
						if (!exp_ty->same(rett)) {
							// TODO: Add message?
							rep::err_stream::report(
							rep::type_mismatch(
								m_File,
								ret_dest->get_return_type_pos(),
								st->Position)
							);
						}
					}
					else {
						ret_dest->set_return_type(rett, st->Position);
					}
				}
				else {
					check_expr(Expression);
				}
				return;
			}
			Otherwise() {
				throw std::exception("Unhandled visit for semantic check (statement)!");
			}
		}
	}

	ysptr<type_symbol> semantic_checker::check_expr(ysptr<expr> ex) {
		Match(ex.get()) {
			Case(unit_expr) {
				return symbol_table::UNIT_T;
			}
			Case(ident_expr, Identifier) {
				auto set = m_Table.ref_filter<typed_symbol>(Identifier);
				bool hint_changed = false;
				if (auto hint = ex->HintType) {
					auto prevsz = set.size();
					set = symbol_table::filter_typed_match(set, hint);
					hint_changed = prevsz != set.size();
				}
				if (set.empty()) {
					if (hint_changed) {
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
				// Choose the last one, this enables shadowing
				auto sym = set[set.size() - 1];
				return sym->Type;
			}
			Case(int_lit_expr, Value) {
				return symbol_table::I32_T;
			}
			Case(real_lit_expr, Value) {
				return symbol_table::F32_T;
			}
			Case(preury_expr, Operator, Sub) {
				auto subt = check_expr(Sub);
				auto sym_name = const_typed_symbol::
				create_preury_op_name(ytoken_t(Operator.Type));
				auto sym_set = m_Table.ref_filter<const_typed_symbol>(sym_name);
				// Create dummy type
				auto dummy_ty = m_Table.create_preury_op_type(subt, symbol_table::UNIT_T);
				// Filter with dummy
				sym_set = symbol_table::filter_typed_match(sym_set, dummy_ty);
				if (sym_set.empty()) {
					throw std::exception("TODO: no such preury operator!");
				}
				else if (sym_set.size() > 1) {
					throw std::exception("Sanity error: multiple preury operators match");
				}
				else {
					auto op = sym_set[0];
					if (auto ty = std::dynamic_pointer_cast<fn_type_symbol>(op->Type)) {
						return ty->Return;
					}
					else {
						throw std::exception("Sanity exception: Not function symbol passed (preury)");
					}
				}
			}
			Case(postury_expr, Operator, Sub) {
				auto subt = check_expr(Sub);
				auto sym_name = const_typed_symbol::
				create_postury_op_name(ytoken_t(Operator.Type));
				auto sym_set = m_Table.ref_filter<const_typed_symbol>(sym_name);
				// Create dummy type
				auto dummy_ty = m_Table.create_postury_op_type(subt, symbol_table::UNIT_T);
				// Filter with dummy
				sym_set = symbol_table::filter_typed_match(sym_set, dummy_ty);
				if (sym_set.empty()) {
					throw std::exception("TODO: no such postury operator!");
				}
				else if (sym_set.size() > 1) {
					throw std::exception("Sanity error: multiple postury operators match");
				}
				else {
					auto op = sym_set[0];
					if (auto ty = std::dynamic_pointer_cast<fn_type_symbol>(op->Type)) {
						return ty->Return;
					}
					else {
						throw std::exception("Sanity exception: Not function symbol passed (postury)");
					}
				}
			}
			Case(binop_expr, Operator, LHS, RHS) {
				auto left = check_expr(LHS);
				auto right = check_expr(RHS);
				auto sym_name = const_typed_symbol::
				create_bin_op_name(ytoken_t(Operator.Type));
				auto sym_set = m_Table.ref_filter<const_typed_symbol>(sym_name);
				// Create dummy type
				auto dummy_ty = m_Table.create_bin_op_type(left, right, symbol_table::UNIT_T);
				// Filter with dummy
				sym_set = symbol_table::filter_typed_match(sym_set, dummy_ty);
				if (sym_set.empty()) {
					// TODO: this is a test
					//rep::code_printer::print(m_File, ex->Position);
					throw std::exception("TODO: no such binary operator!");
				}
				else if (sym_set.size() > 1) {
					throw std::exception("Sanity error: multiple binary operators match");
				}
				else {
					auto op = sym_set[0];
					if (auto ty = std::dynamic_pointer_cast<fn_type_symbol>(op->Type)) {
						return ty->Return;
					}
					else {
						throw std::exception("Sanity exception: Not function symbol passed (binop)");
					}
				}
			}
			Case(asgn_expr, LHS, RHS) {
				auto left = check_expr(LHS);
				auto right = check_expr(RHS);
				if (auto l_unk =
					std::dynamic_pointer_cast<unknown_type_symbol>(left)) {
					if (auto r_unk =
						std::dynamic_pointer_cast<unknown_type_symbol>(right)) {
						throw std::exception("TODO: both left and right are unknown!");
					}
					else {
						l_unk->Owner->Type = right;
					}
				}
				else if (!left->same(right)) {
					throw std::exception("TODO: Assignment type mismatch!");
				}
				return symbol_table::UNIT_T;
			}
			Case(const_asgn_expr, LHS, RHS) {
				auto left = std::dynamic_pointer_cast<ident_expr>(LHS);
				auto right = check_expr(RHS);
				auto sym_set = m_Table.ref_filter<const_typed_symbol>(left->Identifier);
				sym_set = symbol_table::filter_typed_match(sym_set, right);
				if (!sym_set.empty()) {
					std::string err = "TODO: cannot shadow constant assignment: " + left->Identifier;
					throw std::exception(err.c_str());
				}
				auto sym = std::make_shared
					<const_typed_symbol>(left->Identifier, right);
				m_Table.decl(sym);
				return symbol_table::UNIT_T;
			}
			Case(list_expr, Elements) {
				yvec<ysptr<type_symbol>> types;
				for (auto e : Elements) {
					types.push_back(check_expr(e));
				}
				auto sym = std::make_shared<tuple_type_symbol>(types);
				return m_Table.decl_type_once(sym);
			}
			Case(call_expr, Function, Args) {
				ysptr<type_symbol> args_ty = nullptr;
				if (Args) {
					args_ty = check_expr(Args);
				}
				else {
					args_ty = symbol_table::UNIT_T;
				}
				// Create dummy type
				auto dummy_ty = std::make_shared
					<fn_type_symbol>(args_ty, symbol_table::UNIT_T);
				// Help call with filtering
				Function->HintType = dummy_ty;
				auto fn_ty = check_expr(Function);
				if (auto tt = std::dynamic_pointer_cast<fn_type_symbol>(fn_ty)) {
					return tt->Return;
				}
				else {
					throw std::exception("TODO: cannot call non-function expression!");
				}
			}
			Case(block_expr, Statements, ReturnDestination, Scope) {
				auto sc = std::make_shared<scope>();
				Scope = sc;
				if (ReturnDestination) {
					sc->mark_return_dest();
				}
				m_Table.push(sc);
				for (auto st : Statements) {
					check_stmt(st);
				}
				m_Table.pop();
				if (auto rett = sc->get_return_type()) {
					return rett;
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
				// Functions stop returning anyways
				auto sc = std::make_shared<scope>();
				sc->mark_return_dest();
				m_Table.push(sc);

				// Parameters ////////////////////////////////////////
				yvec<ysptr<type_symbol>> params;
				for (auto par : Parameters) {
					auto par_ty = check_type(par.second);
					params.push_back(par_ty);
					if (auto name = par.first) {
						auto par_sym = std::make_shared
							<var_typed_symbol>(name.value().Value, par_ty);
						m_Table.decl(par_sym);
					}
				}
				//////////////////////////////////////////////////////

				// Return type
				ysptr<type_symbol> ret = nullptr;
				
				if (ReturnType) {
					ret = check_type(ReturnType);
				}
				else {
					ret = symbol_table::UNIT_T;
				}

				// Create the type
				ysptr<type_symbol> fn_ty;
				if (params.empty()) {
					fn_ty = std::make_shared<fn_type_symbol>
						(symbol_table::UNIT_T, ret);
				}
				else if (params.size() > 1) {
					ysptr<type_symbol> par_tpl =
						std::make_shared<tuple_type_symbol>(params);
					par_tpl = m_Table.decl_type_once(par_tpl);
					fn_ty = std::make_shared<fn_type_symbol>
						(par_tpl, ret);
				}
				else {
					fn_ty = std::make_shared<fn_type_symbol>
						(params[0], ret);
				}

				auto body_t = check_expr(Body);
				m_Table.pop();

				// Check if body returned what return type needs
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

				fn_ty = m_Table.decl_type_once(fn_ty);
				return fn_ty;
			}
			Case(let_expr, Pattern, Type, Value) {
				ysptr<type_symbol> fin_sym = nullptr;
				ysptr<type_symbol> ty_sym = nullptr;
				ysptr<type_symbol> val_sym = nullptr;
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
					if (!ty_sym->same(val_sym)) {
						throw std::exception("TODO: let type-value mismatch!");
					}
				}
				auto entries = match_pattern_expr(Pattern, fin_sym);
				for (auto e : entries) {
					if (e.second) {
						auto var_sym = std::make_shared
							<var_typed_symbol>(e.first, e.second);
						m_Table.decl(var_sym);
					}
					else {
						auto var_sym = std::make_shared
							<var_typed_symbol>(e.first, nullptr);
						auto unk_ty = std::make_shared<unknown_type_symbol>(var_sym);
						var_sym->Type = unk_ty;
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

	ysptr<type_symbol> semantic_checker::check_type(ysptr<type_desc> ty) {
		Match(ty.get()) {
			Case(unit_type_desc) {
				return symbol_table::UNIT_T;
			}
			Case(ident_type_desc, Identifier) {
				auto t_set = m_Table.ref_filter<type_symbol>(Identifier);
				if (t_set.empty()) {
					throw std::exception("TODO: semantic error (undefined type)");
				}
				if (t_set.size() > 1) {
					throw std::exception("Sanity error: multiple types with same name");
				}
				return t_set[0];
			}
			Case(bin_type_desc, Operator, LHS, RHS) {
				if (Operator.Type == ytoken_t::Arrow) {
					auto left = check_type(LHS);
					auto right = check_type(RHS);
					auto sym = std::make_shared<fn_type_symbol>(left, right);
					return m_Table.decl_type_once(sym);
				}
				else {
					throw std::exception("TODO: no such type operator");
				}
			}
			Case(list_type_desc, Elements) {
				yvec<ysptr<type_symbol>> syms;
				for (auto& el : Elements) {
					syms.push_back(check_type(el));
				}
				auto sym = std::make_shared<tuple_type_symbol>(syms);
				return m_Table.decl_type_once(sym);
			}
			Otherwise() {
				throw std::exception("Unhandled visit for semantic check (type)!");
			}
		}
	}

	yvec<ypair<ystr, ysptr<type_symbol>>> semantic_checker::match_pattern_expr(
		ysptr<pattern> pat, ysptr<type_symbol> ty) {
		yvec<ypair<ystr, ysptr<type_symbol>>> res;
		match_pattern_expr_impl(res, pat, ty, 
			ty.get() != nullptr);
		return res;
	}

	void semantic_checker::match_pattern_expr_impl(
		yvec<ypair<ystr, ysptr<type_symbol>>>& res,
		ysptr<pattern> pat, ysptr<type_symbol> ty,
		bool c_type) {
		Match(pat.get()) {
			Case(ignore_pattern) {
				return;
			}
			Case(unit_pattern) {
				if (c_type) {
					if (!symbol_table::UNIT_T->same(ty)) {
						throw std::exception
						("TODO: Pattern match failed for unit! (type)");
					}
				}
				else {
					throw std::exception
					("TODO: Unmeaningful bind without type!");
				}
				return;
			}
			Case(ident_pattern, Identifier) {
				// Just bind
				if (c_type) {
					res.push_back({ Identifier, ty });
				}
				else {
					res.push_back({ Identifier, nullptr });
				}
				return;
			}
			Case(list_pattern, Elements) {
				if (c_type) {
					if (auto tt = std::dynamic_pointer_cast<tuple_type_symbol>(ty)) {
						if (tt->Elements.size() != Elements.size()) {
							throw std::exception("TODO: Pattern list size mismatch!");
						}
						for (ysize i = 0; i < Elements.size(); i++) {
							match_pattern_expr_impl(res, Elements[i], tt->Elements[i], true);
						}
					}
					else {
						throw std::exception("TODO: Cannot match non-tuple with tuple");
					}
				}
				else {
					for (ysize i = 0; i < Elements.size(); i++) {
						match_pattern_expr_impl(res, Elements[i], nullptr, false);
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
