#include "semantic_checker.h"

namespace yk {
	semantic_checker::semantic_checker()
		: m_StmtChecker(*this, m_SymbolTable), m_ExprChecker(*this, m_SymbolTable), 
		m_TypeChecker(*this, m_SymbolTable) {
	}

	void semantic_checker::check(yvec<stmt*> const& prog) {
		for (auto s : prog) {
			check_stmt(s);
		}
	}

	void semantic_checker::check_stmt(stmt* st) {
		m_StmtChecker.dispatch_gen(st);
	}

	type_symbol* semantic_checker::check_expr(expr* exp) {
		return m_ExprChecker.dispatch_gen(exp);
	}

	type_symbol* semantic_checker::check_type(type_desc* ty) {
		return m_TypeChecker.dispatch_gen(ty);
	}
}