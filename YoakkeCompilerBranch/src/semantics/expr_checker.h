#pragma once

#include "../ast/expr.h"
#include "../utility/visitor.h"

namespace yk {
	class semantic_checker;
	class symbol_table;

	class expr_checker : public visitor<expr, type_symbol*> {
	public:
		META_Visitor(expr_checker, check)

	private:
		semantic_checker& m_Checker;
		symbol_table& m_Table;

	public:
		expr_checker(semantic_checker& ch, symbol_table& tab);

	public:
		type_symbol* check(expr& exp);
		type_symbol* check(ident_expr& exp);
		type_symbol* check(unit_expr& exp);
		type_symbol* check(int_lit_expr& exp);
		type_symbol* check(real_lit_expr& exp);
		type_symbol* check(bin_expr& exp);
		type_symbol* check(preury_expr& exp);
		type_symbol* check(postury_expr& exp);
		type_symbol* check(list_expr& exp);
		type_symbol* check(mixfix_expr& exp);
		type_symbol* check(func_proto& exp);
		type_symbol* check(func_expr& exp);
		type_symbol* check(body_expr& exp);
		type_symbol* check(param_expr& exp);
		type_symbol* check(let_expr& exp);
		type_symbol* check(const_asgn_expr& exp);
	};
}