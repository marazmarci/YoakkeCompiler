#pragma once

#include "ast_node.h"
#include "../utility/optional.h"

namespace yk {
	class expr;

	class stmt : public ast_node {
	protected:
		stmt(position const& pos);

	public:
		virtual ~stmt();
	};

	class expr_stmt : public stmt {
	public:
		expr* Sub;
		yopt<token> Semicol;

	public:
		expr_stmt(expr* sub);
		expr_stmt(expr* sub, token const& sc);
		virtual ~expr_stmt();
	};
}