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

	// Prefix unary expression
	preury_expr::preury_expr(expr* s, token const& o)
		: expr(position::interval(position::get(o), s->Position)), Sub(s), OP(o) {
	}

	preury_expr::~preury_expr() {
		delete Sub;
	}

	// Postfix unary expression
	postury_expr::postury_expr(expr* s, token const& o)
		: expr(position::interval(s->Position, position::get(o))), Sub(s), OP(o) {
	}

	postury_expr::~postury_expr() {
		delete Sub;
	}

	// Enclosed expression
	enclose_expr::enclose_expr(expr* s, token const& beg, token const& end)
		: expr(position::interval(position::get(beg), position::get(end))),
		Sub(s), Begin(beg), End(end) {
	}

	enclose_expr::~enclose_expr() {
		delete Sub;
	}
}