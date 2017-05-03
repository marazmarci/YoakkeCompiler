#include "pat_expr.h"

namespace yk {
	// Abstract pattern

	pat_expr::pat_expr(interval const& pos)
		: node(pos) {
	}

	pat_expr::~pat_expr() { }

	// Ignore pattern

	ignore_pat_expr::ignore_pat_expr(token const& tok)
		: pat_expr(tok.Position) {
	}

	ignore_pat_expr::~ignore_pat_expr() { }

	// Unit pattern

	unit_pat_expr::unit_pat_expr(token const& beg, token const& end)
		: pat_expr(interval(beg.Position, end.Position)) {
	}

	unit_pat_expr::~unit_pat_expr() { }

	// Identifier pattern

	ident_pat_expr::ident_pat_expr(token const& tok)
		: pat_expr(tok.Position), Identifier(tok.Value) {
	}

	ident_pat_expr::~ident_pat_expr() { }

	// Binary pattern

	bin_pat_expr::bin_pat_expr(token const& op, ysptr<pat_expr> lhs, ysptr<pat_expr> rhs)
		: pat_expr(interval(lhs->Position, rhs->Position)),
		Operator(op), LHS(lhs), RHS(rhs) {
	}

	bin_pat_expr::~bin_pat_expr() { }

	// List pattern

	list_pat_expr::list_pat_expr(ysptr<pat_expr>& left)
		: pat_expr(left->Position) {
		Elements.push_back(left);
	}

	list_pat_expr::~list_pat_expr() { }

	void list_pat_expr::add(ysptr<pat_expr>& exp) {
		Elements.push_back(exp);
		Position.End = exp->Position.End;
	}
}
