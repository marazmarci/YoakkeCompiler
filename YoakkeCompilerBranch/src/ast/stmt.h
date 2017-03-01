#pragma once

#include "ast_node.h"
#include "../utility/optional.h"

namespace yk {
	class expr;

	class stmt : public ast_node, public visitable<stmt> {
	public:
		META_BaseVisitable(stmt)

	protected:
		stmt(position const& pos);

	public:
		virtual ~stmt();
	};

	class expr_stmt : public stmt {
	public:
		META_Visitable(expr_stmt, stmt)

	public:
		expr* Sub;
		yopt<token> Semicol;

	public:
		expr_stmt(expr* sub);
		expr_stmt(expr* sub, token const& sc);
		virtual ~expr_stmt();
	};
}