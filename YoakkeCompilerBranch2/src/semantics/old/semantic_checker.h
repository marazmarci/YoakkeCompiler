#pragma once

#include "symbol_table.h"
#include "../ast/expr.h"
#include "../utility/file_handle.h"

namespace yk {
	class stmt;
	class expr;
	class pat_expr;
	class ty_expr;
	class type_symbol;

	class semantic_checker {
	private:
		symbol_table m_Table;
		file_handle const& m_File;

	public:
		semantic_checker(file_handle const& f);

	public:
		void				check_stmt(ysptr<stmt> st);
		ysptr<type_symbol>	check_expr(ysptr<expr> ex);
		ysptr<type_symbol>	check_type(ysptr<ty_expr> ty);

	private:
		yvec<ypair<ystr, ysptr<type_symbol>>> match_pattern_expr(ysptr<pat_expr> pat, ysptr<type_symbol> ty);
		void match_pattern_expr_impl(yvec<ypair<ystr, ysptr<type_symbol>>>& res, ysptr<pat_expr> pat, ysptr<type_symbol> ty, bool c_type);
	};
}