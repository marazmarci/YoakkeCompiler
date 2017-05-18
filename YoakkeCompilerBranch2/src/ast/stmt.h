#pragma once

/*
These are all the statement nodes of the AST.
*/

#include "node.h"
#include "../lexing/token.h"

namespace yk {
	class expr;

	/*
	Statement base-class.
	*/
	class stmt : public node {
	protected:
		stmt(interval const& pos);

	public:
		virtual ~stmt();
	};

	/*
	A statement containing an expression that may or may not contain 
	a semicolon at the end (or return the value).
	*/
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
