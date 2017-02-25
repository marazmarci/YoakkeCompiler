#pragma once

#include "../ast/stmt.h"
#include "../utility/double_dispatcher.h"

namespace yk {
	class semantic_checker;

	class stmt_checker : public double_dispatcher
		<void, stmt,
		expr_stmt> {
	private:
		semantic_checker* m_Checker;

	public:
		stmt_checker(semantic_checker* ch);

	public:
		void dispatch(expr_stmt* stmt) override;
	};
}