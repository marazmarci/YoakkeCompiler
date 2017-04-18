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
		mch::var<ysptr<expr>> exp1, exp2;
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
						throw std::exception("TODO: no such symbol");
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
				// TODO
			}
			Case(asgn_expr, exp1, exp2) {
				// TODO
			}
			Case(const_asgn_expr, exp1, exp2) {
				// TODO
			}
			Case(list_expr, exp_list) {
				// TODO
			}
			Case(call_expr, exp1, exp2) {
				// TODO
			}
			Case(block_expr, stmt_list) {
				// TODO
			}
			Case(fnproto_expr, param_list, type) {
				// TODO
			}
			Case(fn_expr, proto, block) {
				// TODO
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
