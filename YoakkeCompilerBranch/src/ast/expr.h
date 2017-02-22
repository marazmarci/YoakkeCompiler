#pragma once

#include "ast_node.h"
#include "../parse/token.h"

namespace yk {
	class expr : public ast_node {
	protected:
		expr(position const& pos);

	public:
		virtual ~expr();
	};

	class ident_expr : public expr {
	public:
		const ystr identifier;

	public:
		ident_expr(token const& tok);
		virtual ~ident_expr();
	};

	class bin_expr : public expr {
	public:
		expr* LHS;
		expr* RHS;
		token OP;

	public:
		bin_expr(expr* l, expr* r, token const& o);
		virtual ~bin_expr();
	};

	class preury_expr : public expr {
	public:
		expr* Sub;
		token OP;

	public:
		preury_expr(expr* s, token const& o);
		virtual ~preury_expr();
	};

	class postury_expr : public expr {
	public:
		expr* Sub;
		token OP;

	public:
		postury_expr(expr* s, token const& o);
		virtual ~postury_expr();
	};

	class enclose_expr : public expr {
	public:
		token Begin;
		token End;
		expr* Sub;

	public:
		enclose_expr(expr* s, token const& beg, token const& end);
		virtual ~enclose_expr();
	};
}