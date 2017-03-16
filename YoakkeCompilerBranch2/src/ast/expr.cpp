#include "expr.h"

namespace yk {
	expr::expr(interval const& pos)
		: node(pos) {
	}

	expr::~expr() { }

	// Identifier expression

	ident_expr::ident_expr(token const& tok)
		: expr(tok.Position), Identifier(tok.Value) {
	}
	
	ident_expr::~ident_expr() { }
}
