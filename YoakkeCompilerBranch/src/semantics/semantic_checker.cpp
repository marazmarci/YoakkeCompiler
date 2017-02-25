#include "semantic_checker.h"

namespace yk {
	semantic_checker::semantic_checker()
		: m_StmtChecker(this), m_ExprChecker(this), m_TypeChecker(this) {
	}

	void semantic_checker::check(yvec<stmt*> const& prog) {
		for (auto s : prog) {
			check_stmt(s);
		}
	}

	void semantic_checker::check_stmt(stmt* st) {
		m_StmtChecker.dispatch_gen(st);
	}

	void semantic_checker::check_expr(expr* exp) {
		m_ExprChecker.dispatch_gen(exp);
	}

	void semantic_checker::check_type(type_desc* ty) {
		m_TypeChecker.dispatch_gen(ty);
	}
}