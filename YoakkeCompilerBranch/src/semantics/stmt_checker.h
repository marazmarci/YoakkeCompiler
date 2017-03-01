#pragma once

#include "../ast/stmt.h"
#include "../utility/visitor.h"

namespace yk {
	class semantic_checker;
	class symbol_table;

	class stmt_checker : public visitor<stmt, void> {
	public:
		META_Visitor(stmt_checker, check)

	private:
		semantic_checker& m_Checker;
		symbol_table& m_Table;

	public:
		stmt_checker(semantic_checker& ch, symbol_table& tab);

	public:
		void check(stmt& st);
		void check(expr_stmt& st);
	};
}