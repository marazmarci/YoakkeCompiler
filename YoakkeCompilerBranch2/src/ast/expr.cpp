#include <cstdlib>
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

	// Integer literal expression

	int_lit_expr::int_lit_expr(token const& tok)
		: expr(tok.Position), Value(std::atoi(tok.Value.c_str())) {
	}

	int_lit_expr::~int_lit_expr() { }

	// Real literal expression

	real_lit_expr::real_lit_expr(token const& tok)
		: expr(tok.Position), Value(std::atof(tok.Value.c_str())) {
	}

	real_lit_expr::~real_lit_expr() { }

	// Prefix unary expression

	preury_expr::preury_expr(token const& op, yshared_ptr<expr> sub)
		: expr(interval(op.Position, sub->Position)),
		Operator(op), Sub(sub) {
	}

	preury_expr::~preury_expr() { }

	// Post unary  expression

	postury_expr::postury_expr(token const& op, yshared_ptr<expr> sub)
		: expr(interval(sub->Position, op.Position)),
		Operator(op), Sub(sub) {
	}

	postury_expr::~postury_expr() { }

	// Binary expression

	bin_expr::bin_expr(token const& op, yshared_ptr<expr> lhs, yshared_ptr<expr> rhs)
		: expr(interval(lhs->Position, rhs->Position)),
		Operator(op), LHS(lhs), RHS(rhs) {
	}

	bin_expr::~bin_expr() { }
}
