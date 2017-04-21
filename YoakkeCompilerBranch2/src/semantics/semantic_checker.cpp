#include "semantic_checker.h"
#include "type_symbol.h"
#include "../lexing/ytoken_t.h"
#include "typed_symbol.h"
#include "../ast/stmt.h"
#include "../ast/expr.h"
#include "../ast/type_desc.h"
#include "../ast/pattern.h"
#include "../utility/match.h"

namespace yk {
	void semantic_checker::check_stmt(ysptr<stmt> st) {
		Match(st.get())
			Case(expr_stmt, Expression, Semicol)
				check_expr(Expression);
				return;
			EndCase
			CaseOther()
				throw std::exception("Unhandled visit for semantic check (statement)!");
			EndCase
		EndMatch
	}

	ysptr<type_symbol> semantic_checker::check_expr(ysptr<expr> ex) {
		Match(ex.get()) 
			Case(unit_expr)
				return symbol_table::UNIT_T;
			EndCase
			Case(ident_expr, Identifier)
				auto set = m_Table.ref_filter<typed_symbol>(Identifier);
				bool hint_changed = false;
				if (auto hint = ex->HintType) {
					auto prevsz = set.size();
					set = symbol_table::filter_typed_match(set, hint);
					hint_changed = prevsz != set.size();
				}
				if (set.empty()) {
					if (hint_changed) {
						throw std::exception("TODO: no such symbol (hint mismatch?)");
					}
					else {
						std::string err = "TODO: no such symbol: " + Identifier;
						throw std::exception(err.c_str());
					}
				}
				// Choose the last one, this enables shadowing
				auto sym = set[set.size() - 1];
				return sym->Type;
			EndCase
			Case(int_lit_expr, Value)
				return symbol_table::I32_T;
			EndCase
			Case(real_lit_expr, Value)
				return symbol_table::F32_T;
			EndCase
			Case(preury_expr, Operator, Sub)
				// TODO
			EndCase
			Case(postury_expr, Operator, Sub)
				// TODO
			EndCase
			Case(binop_expr, Operator, LHS, RHS)
				auto left = check_expr(LHS);
				auto right = check_expr(RHS);
				auto sym_name = const_typed_symbol::create_bin_op_name(ytoken_t(Operator.Type));
				auto sym_set = m_Table.ref_filter<const_typed_symbol>(sym_name);
				// Create dummy type
				auto dummy_ty = m_Table.create_bin_op_type(left, right, symbol_table::UNIT_T);
				// Filter with dummy
				sym_set = symbol_table::filter_typed_match(sym_set, dummy_ty);
				if (sym_set.empty()) {
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
			EndCase
			Case(asgn_expr, LHS, RHS)
				// TODO
			EndCase
			Case(const_asgn_expr, LHS, RHS)
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
			EndCase
			Case(list_expr, Elements)
				yvec<ysptr<type_symbol>> types;
				for (auto e : Elements) {
					types.push_back(check_expr(e));
				}
				auto sym = std::make_shared<tuple_type_symbol>(types);
				return m_Table.decl_type_once(sym);
			EndCase
			Case(call_expr, Function, Args)
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
			EndCase
			Case(block_expr, Statements)
				// TODO: return actual type
				m_Table.push(std::make_shared<scope>());
				for (auto st : Statements) {
					check_stmt(st);
				}
				m_Table.pop();
				return symbol_table::UNIT_T;
			EndCase
			Case(fnproto_expr, Parameters, ReturnType)
				// TODO
			EndCase
			Case(fn_expr, Prototype, Body)
				// TODO: warn if no parameter name
				m_Table.push(std::make_shared<scope>());

				// Parameters ////////////////////////////////////////
				yvec<ysptr<type_symbol>> params;
				for (auto par : Prototype->Parameters) {
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
				ysptr<type_symbol> ret;

				if (Prototype->ReturnType) {
					ret = check_type(Prototype->ReturnType);
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

				check_expr(Body);
				m_Table.pop();

				fn_ty = m_Table.decl_type_once(fn_ty);
				return fn_ty;
			EndCase
			Case(let_expr, Pattern, Type, Value)
				ysptr<type_symbol> fin_sym = nullptr;
				ysptr<type_symbol> ty_sym = nullptr;
				ysptr<type_symbol> val_sym = nullptr;
				if (Type) {
					ty_sym = check_type(Type);
					if (Value) {
						Value->HintType = ty_sym;
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
					// TODO: type is null?
					auto var_sym = std::make_shared
						<var_typed_symbol>(e.first, e.second);
					m_Table.decl(var_sym);
				}
				return symbol_table::UNIT_T;
			EndCase
			CaseOther()
				throw std::exception("Unhandled visit for semantic check (expression)!");
			EndCase
		EndMatch
	}

	ysptr<type_symbol> semantic_checker::check_type(ysptr<type_desc> ty) {
		Match(ty.get()) 
			Case(unit_type_desc)
				return symbol_table::UNIT_T;
			EndCase
			Case(ident_type_desc, Identifier)
				auto t_set = m_Table.ref_filter<type_symbol>(Identifier);
				if (t_set.empty()) {
					throw std::exception("TODO: semantic error (undefined type)");
				}
				if (t_set.size() > 1) {
					throw std::exception("Sanity error: multiple types with same name");
				}
				return t_set[0];
			EndCase
			Case(bin_type_desc, Operator, LHS, RHS)
				if (Operator.Type == (ysize)ytoken_t::Arrow) {
					auto left = check_type(LHS);
					auto right = check_type(RHS);
					auto sym = std::make_shared<fn_type_symbol>(left, right);
					return m_Table.decl_type_once(sym);
				}
				else {
					throw std::exception("TODO: no such type operator");
				}
			EndCase
			Case(list_type_desc, Elements)
				yvec<ysptr<type_symbol>> syms;
				for (auto& el : Elements) {
					syms.push_back(check_type(el));
				}
				auto sym = std::make_shared<tuple_type_symbol>(syms);
				return m_Table.decl_type_once(sym);
			EndCase
			CaseOther()
				throw std::exception("Unhandled visit for semantic check (type)!");
			EndCase
		EndMatch
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
		Match(pat.get())
			Case(ignore_pattern)
				return;
			EndCase
			Case(unit_pattern)
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
			EndCase
			Case(ident_pattern, Identifier)
				// Just bind
				if (c_type) {
					res.push_back({ Identifier, ty		});
				}
				else {
					res.push_back({ Identifier, nullptr });
				}
				return;
			EndCase
			Case(list_pattern, Elements)
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
			EndCase
			CaseOther()
				throw std::exception("Unhandled visit for pattern matching (expression)!");
			EndCase
		EndMatch
	}
}
