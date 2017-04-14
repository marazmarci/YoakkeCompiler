#include "semantic_checker.h"
#include "type_symbol.h"
#include "../ast/mch_bindings.h"
#include "../lexing/ytoken_t.h"

namespace yk {
	void semantic_checker::check_stmt(yshared_ptr<stmt> st) {
		mch::var<yshared_ptr<expr>> expression;
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

	yshared_ptr<type_symbol> semantic_checker::check_expr(yshared_ptr<expr> ex) {
		mch::var<ystr> str;
		mch::var<long long> ival;
		mch::var<long double> dval;
		mch::var<token> tok;
		mch::var<expr> exp1, exp2;
		mch::var<yvec<yshared_ptr<expr>>> exp_list;
		mch::var<yshared_ptr<pattern>> patt;
		mch::var<yvec<yshared_ptr<stmt>>> stmt_list;
		mch::var<yvec<fnproto_expr::param_t>> param_list;
		mch::var<yshared_ptr<fnproto_expr>> proto;
		mch::var<yshared_ptr<type_desc>> type;
		mch::var<yshared_ptr<block_expr>> block;

		Match(*ex.get()) 
		{
			Case(unit_expr) {
				return symbol_table::UNIT_T;
			}
			Case(ident_expr, str) {
				// TODO
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

	yshared_ptr<type_symbol> semantic_checker::check_type(yshared_ptr<type_desc> ty) {
		mch::var<ystr> str;
		mch::var<token> tok;
		mch::var<yshared_ptr<type_desc>> type1, type1;
		mch::var<yvec<yshared_ptr<type_desc>>> type_list;

		Match(*ty.get()) {
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
					ystr t_name = fn_type_symbol::create_name(left, right);
					auto t_set = m_Table.ref_global_filter<type_symbol>(t_name);
					if (t_set.empty()) {
						// Create the symbol
						auto sym = std::make_shared<fn_type_symbol>(left, right);
						m_Table.decl_global(sym);
					}
					else if (t_set.size() > 1) {
						// Sanity error
						throw std::exception("Sanity error: multiple definitions of the same function type");
					}
					else {
						// Already exists one, grab it
						return t_set[0];
					}
				}
				else {
					throw std::exception("TODO: no such type operator");
				}
			}
			Case(list_type_desc, type_list) {
				yvec<yshared_ptr<type_symbol>> syms;
				for (auto& el : type_list) {
					syms.push_back(check_type(el));
				}
				ystr t_name = tuple_type_symbol::create_name(syms);
				auto t_set = m_Table.ref_global_filter<type_symbol>(t_name);
				if (t_set.empty()) {
					// Create the symbol
					auto sym = std::make_shared<tuple_type_symbol>(syms);
					m_Table.decl_global(sym);
				}
				else if (t_set.size() > 1) {
					// Sanity error
					throw std::exception("Sanity error: multiple definitions of the same function type");
				}
				else {
					// Already exists one, grab it
					return t_set[0];
				}
			}
			Otherwise() {
				throw std::exception("Unhandled visit for semantic check (type)!");
			}
		}
	}
}
