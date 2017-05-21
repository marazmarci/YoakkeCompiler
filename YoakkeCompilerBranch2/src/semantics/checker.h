#pragma once

#include "symbol_table.h"
#include "../utility/file_handle.h"
#include "../common.h"

namespace yk {
	struct stmt;
	struct expr;
	struct pat_expr;
	struct ty_expr;
	struct type;

	class checker {
	private:
		symbol_table m_Table;
		file_handle const& m_File;

	public:
		checker(file_handle const& f);

	public:
		void				check_stmt(stmt& st);
		type				check_expr(expr& ex);
		type				check_type(ty_expr& ty);

	private:
		bool* get_braced(expr const& e);

		yvec<ypair<ystr, yopt<type>>> match_pat(pat_expr const& pat, yopt<type> ty);
		void match_pat_impl(yvec<ypair<ystr, yopt<type>>>& res, pat_expr const& pat, yopt<type> ty);
	};
}
