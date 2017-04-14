#pragma once

#include "symbol_table.h"

namespace yk {
	class stmt;
	class expr;
	class pattern;
	class type_desc;
	class type_symbol;

	class semantic_checker {
	private:
		symbol_table m_Table;

	public:
		void check_stmt(yshared_ptr<stmt> st);
		yshared_ptr<type_symbol> check_expr(yshared_ptr<expr> ex);
		yshared_ptr<type_symbol> check_type(yshared_ptr<type_desc> ty);
	};
}
