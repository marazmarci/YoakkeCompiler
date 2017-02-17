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
}