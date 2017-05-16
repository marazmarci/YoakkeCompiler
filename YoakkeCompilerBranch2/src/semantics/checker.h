#pragma once

#include "symbol_table.h"
#include "../utility/file_handle.h"
#include "../common.h"

namespace yk {
	class stmt;
	class expr;
	class pat_expr;
	class ty_expr;
	class type;

	class checker {
	private:
		symbol_table m_Table;
		file_handle const& m_File;

	public:
		checker(file_handle const& f);

	public:
		void				check_stmt(ysptr<stmt> st);
		ysptr<type>			check_expr(ysptr<expr> ex);
		ysptr<type>			check_type(ysptr<ty_expr> ty);

	private:
		ysptr<type> prune(ysptr<type> t);
		void unify(ysptr<type> t1, ysptr<type> t2);

		yvec<ypair<ystr, ysptr<type>>> match_pat(ysptr<pat_expr> pat, ysptr<type> ty);
		void match_pat_impl(yvec<ypair<ystr, ysptr<type>>>& res, ysptr<pat_expr> pat, ysptr<type> ty);
	};
}
