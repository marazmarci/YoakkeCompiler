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
		const expr* LHS;
		const expr* RHS;
		const token OP;

	public:
		bin_expr(expr* l, expr* r, token const& o);
		~bin_expr();
	};
}