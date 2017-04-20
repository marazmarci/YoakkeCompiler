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
		yvec<ypair<ystr, ysptr<type_symbol>>> match_pattern_expr(ysptr<pattern> pat, ysptr<type_symbol> ty);
		void match_pattern_expr_impl(yvec<ypair<ystr, ysptr<type_symbol>>>& res, ysptr<pattern> pat, ysptr<type_symbol> ty, bool c_type);
	};
}
