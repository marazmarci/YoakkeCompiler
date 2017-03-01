#include "stmt_checker.h"
#include "semantic_checker.h"
#include "expr_checker.h"

namespace yk {
	stmt_checker::stmt_checker(semantic_checker& ch, symbol_table& tab)
		: m_Checker(ch), m_Table(tab) {
		META_Visitables(stmt, expr_stmt);
	}

	void stmt_checker::check(stmt& st) {
		throw std::exception("Statement check not covered!");
	}

	void stmt_checker::check(expr_stmt& st) {
		m_Checker.check_expr(st.Sub);
	}
}
