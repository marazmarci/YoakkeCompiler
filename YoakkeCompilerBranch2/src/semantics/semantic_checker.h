#pragma once

#include "symbol_table.h"
#include "../ast/expr.h"

namespace yk {
	class stmt;
	class expr;
	class pattern;
	class type_desc;
	class type_symbol;

	class semantic_checker {
	private:
		symbol_table m_Table;

	public:
		void				check_stmt(ysptr<stmt> st);
		ysptr<type_symbol>	check_expr(ysptr<expr> ex);
		ysptr<type_symbol>	check_type(ysptr<type_desc> ty);

	private:
		yvec<let_expr::entry_t> match_pattern_expr(ysptr<pattern> pat, ysptr<type_symbol> ty, ysptr<expr> exp);
		void match_pattern_expr_impl(yvec<let_expr::entry_t>& res, ysptr<pattern> pat, ysptr<type_symbol> ty, ysptr<expr> exp, bool c_type, bool c_exp);
	};
}
