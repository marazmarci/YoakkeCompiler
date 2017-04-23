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
		ysptr<expr> Expression;
		bool Semicol;
		bool Return;

	public:
		expr_stmt(ysptr<expr>& exp, yopt<token> semicol);
		virtual ~expr_stmt();
	};
}
