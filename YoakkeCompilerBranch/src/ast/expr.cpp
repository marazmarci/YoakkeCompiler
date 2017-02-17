#include "expr.h"

namespace yk {
	// Expression
	expr::expr(position const& pos)
		: ast_node(pos) {
	}

	expr::~expr() { }

	// Identifier expression
	ident_expr::ident_expr(token const& tok)
		: expr(position::get(tok)), identifier(tok.value()) {
	}

	ident_expr::~ident_expr() { }

	// Binary expression
	bin_expr::bin_expr(expr* l, expr* r, token const& o)
		: expr(position::interval(l->Position, r->Position)), LHS(l), RHS(r), OP(o) {
	}

	bin_expr::~bin_expr() {
		delete LHS;
		delete RHS;
	}
}