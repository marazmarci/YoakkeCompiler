#include <cstdlib>
#include "expr.h"

namespace yk {
	// Abstract expression

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

	// List expression

	list_expr::list_expr(yshared_ptr<expr>& left)
		: expr(left->Position) {
		Expressions.push_back(left);
	}

	list_expr::~list_expr() { }

	void list_expr::add(yshared_ptr<expr>& exp) {
		Expressions.push_back(exp);
		Position.End = exp->Position.End;
	}

	// Function call expression

	call_expr::call_expr(yshared_ptr<expr>& fn, yshared_ptr<expr>& args, token const& end)
		: expr(interval(fn->Position, end.Position)), Function(fn), Args(args) {
	}

	call_expr::~call_expr() { }
}
