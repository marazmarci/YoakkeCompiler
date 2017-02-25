#include "stmt_checker.h"
#include "semantic_checker.h"
#include "expr_checker.h"

namespace yk {
	stmt_checker::stmt_checker(semantic_checker& ch, symbol_table& tab)
		: m_Checker(ch), m_Table(tab) {
	}

	void stmt_checker::dispatch(expr_stmt* stmt) {
		m_Checker.check_expr(stmt->Sub);
	}
}
