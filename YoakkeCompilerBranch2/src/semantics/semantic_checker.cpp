#include "semantic_checker.h"
#include "type_symbol.h"
#include "../ast/mch_bindings.h"
#include "../lexing/ytoken_t.h"
#include "typed_symbol.h"

namespace yk {
	void semantic_checker::check_stmt(ysptr<stmt> st) {
		mch::var<ysptr<expr>> expression;
		mch::var<bool> semicol;

		Match(*st.get())
		{
			Case(expr_stmt, expression, semicol) {
				check_expr(expression);
				return;
			}
			Otherwise() {
				throw std::exception("Unhandled visit for semantic check (statement)!");
			}
		}
		EndMatch
	}

	ysptr<type_symbol> semantic_checker::check_expr(ysptr<expr> ex) {
		mch::var<ystr> str;
		mch::var<long long> ival;
		mch::var<long double> dval;
		mch::var<token> tok;
		mch::var<ysptr<expr>> exp1;
		mch::var<ysptr<expr>> exp2;
		mch::var<yvec<ysptr<expr>>> exp_list;
		mch::var<ysptr<pattern>> patt;
		mch::var<yvec<ysptr<stmt>>> stmt_list;
		mch::var<yvec<fnproto_expr::param_t>> param_list;
		mch::var<ysptr<fnproto_expr>> proto;
		mch::var<ysptr<type_desc>> type;
		mch::var<ysptr<block_expr>> block;

		Match(*ex.get()) 
		{
			Case(unit_expr) {
				return symbol_table::UNIT_T;
			}
			Case(ident_expr, str) {
				auto set = m_Table.ref_filter<typed_symbol>(str);
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
						std::string err = "TODO: no such symbol: " + str;
						throw std::exception(err.c_str());
					}
				}
				// Choose the last one, this enables shadowing
				auto sym = set[set.size() - 1];
				return sym->Type;
			}
			Case(int_lit_expr, ival) {
				return symbol_table::I32_T;
			}
			Case(real_lit_expr, dval) {
				return symbol_table::F32_T;
			}
			Case(preury_expr, tok, exp1) {
				// TODO
			}
			Case(postury_expr, tok, exp1) {
				// TODO
			}
			Case(binop_expr, tok, exp1, exp2) {
				auto left = check_expr(exp1);
				auto right = check_expr(exp2);
				auto sym_name = const_typed_symbol::create_bin_op_name(ytoken_t(tok.Type));
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
			}
			Case(asgn_expr, exp1, exp2) {
				// TODO
			}
			Case(const_asgn_expr, exp1, exp2) {
				auto left = std::dynamic_pointer_cast<ident_expr>(exp1);
				auto right = check_expr(exp2);
				auto sym = std::make_shared
					<const_typed_symbol>(left->Identifier, right);
				m_Table.decl(sym);
				return symbol_table::UNIT_T;
			}
			Case(list_expr, exp_list) {
				// TODO
			}
			Case(call_expr, exp1, exp2) {
				// TODO
			}
			Case(block_expr, stmt_list) {
				// TODO: return actual type
				m_Table.push(std::make_shared<scope>());
				for (auto st : stmt_list) {
					check_stmt(st);
				}
				m_Table.pop();
				return symbol_table::UNIT_T;
			}
			Case(fnproto_expr, param_list, type) {
				// TODO
			}
			Case(fn_expr, proto, block) {
				// TODO: warn if no parameter name
				m_Table.push(std::make_shared<scope>());

				// Parameters ////////////////////////////////////////
				yvec<ysptr<type_symbol>> params;
				for (auto par : proto->Parameters) {
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

				if (proto->ReturnType) {
					ret = check_type(proto->ReturnType);
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

				check_expr(block);
				m_Table.pop();

				fn_ty = m_Table.decl_type_once(fn_ty);
				return fn_ty;
			}
			Case(let_expr, patt, type, exp1) {
				// TODO
			}
			Otherwise() {
				throw std::exception("Unhandled visit for semantic check (expression)!");
			}
		}
		EndMatch
	}

	ysptr<type_symbol> semantic_checker::check_type(ysptr<type_desc> ty) {
		mch::var<ystr> str;
		mch::var<token> tok;
		mch::var<ysptr<type_desc>> type1, type2;
		mch::var<yvec<ysptr<type_desc>>> type_list;

		Match(*ty.get()) 
		{
			Case(unit_expr) {
				return symbol_table::UNIT_T;
			}
			Case(ident_type_desc, str) {
				auto t_set = m_Table.ref_filter<type_symbol>(str);
				if (t_set.empty()) {
					throw std::exception("TODO: semantic error (undefined type)");
				}
				if (t_set.size() > 1) {
					throw std::exception("Sanity error: multiple types with same name");
				}
				return t_set[0];
			}
			Case(bin_type_desc, tok, type1, type2) {
				if (tok.Type == (ysize)ytoken_t::Arrow) {
					auto left = check_type(type1);
					auto right = check_type(type2);
					auto sym = std::make_shared<fn_type_symbol>(left, right);
					return m_Table.decl_type_once(sym);
				}
				else {
					throw std::exception("TODO: no such type operator");
				}
			}
			Case(list_type_desc, type_list) {
				yvec<ysptr<type_symbol>> syms;
				for (auto& el : type_list) {
					syms.push_back(check_type(el));
				}
				auto sym = std::make_shared<tuple_type_symbol>(syms);
				return m_Table.decl_type_once(sym);
			}
			Otherwise() {
				throw std::exception("Unhandled visit for semantic check (type)!");
			}
		}
		EndMatch
	}
}
