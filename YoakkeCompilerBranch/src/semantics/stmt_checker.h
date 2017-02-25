#pragma once

#include "../ast/stmt.h"
#include "../utility/double_dispatcher.h"

namespace yk {
	class semantic_checker;
	class symbol_table;

	class stmt_checker : public double_dispatcher
		<void, stmt,
		expr_stmt> {
	private:
		semantic_checker& m_Checker;
		symbol_table& m_Table;

	public:
		stmt_checker(semantic_checker& ch, symbol_table& tab);

	public:
		void dispatch(expr_stmt* stmt) override;
	};
}