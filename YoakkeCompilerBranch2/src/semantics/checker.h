#pragma once

#include "symbol_table.h"
#include "../utility/file_handle.h"
#include "../common.h"

namespace yk {
	struct stmt;
	struct expr;
	struct pat_expr;
	struct ty_expr;
	class type;

	class checker {
	private:
		symbol_table m_Table;
		file_handle const& m_File;

	public:
		checker(file_handle const& f);

	public:
		void				check_stmt(stmt const& st);
		ysptr<type>			check_expr(expr const& ex);
		ysptr<type>			check_type(ty_expr const& ty);

	private:
		bool* get_braced(expr const& e);

		ysptr<type> prune(ysptr<type> t);
		void unify(ysptr<type> t1, ysptr<type> t2);

		yvec<ypair<ystr, ysptr<type>>> match_pat(pat_expr const& pat, ysptr<type> ty);
		void match_pat_impl(yvec<ypair<ystr, ysptr<type>>>& res, pat_expr const& pat, ysptr<type> ty);
	};
}
