#pragma once

#include "stmt_checker.h"
#include "expr_checker.h"
#include "type_checker.h"
#include "symbol_table.h"

namespace yk {
	class semantic_checker {
	private:
		stmt_checker m_StmtChecker;
		expr_checker m_ExprChecker;
		type_checker m_TypeChecker;
		symbol_table m_SymbolTable;

	public:
		semantic_checker();

	public:
		void check(yvec<stmt*> const& prog);
		void check_stmt(stmt* st);
		type_symbol* check_expr(expr* exp);
		type_symbol* check_type(ty_expr* ty);
	};
}