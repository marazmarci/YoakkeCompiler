#pragma once

#include "node.h"
#include "../lexing/token.h"

namespace yk {
	class expr;

	class stmt : public node {
	protected:
		stmt(interval const& pos);

	public:
		virtual ~stmt();
	};

	class expr_stmt : public stmt {
	public:
		yshared_ptr<expr> Expression;
		bool Semicol;

	public:
		expr_stmt(yshared_ptr<expr>& exp, yopt<token> semicol);
		virtual ~expr_stmt();
	};
}
