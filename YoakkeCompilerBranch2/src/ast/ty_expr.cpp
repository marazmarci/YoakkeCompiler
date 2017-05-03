#include "ty_expr.h"

namespace yk {
	// Abstract type descriptor

	ty_expr::ty_expr(interval const& pos)
		: node(pos) {
	}

	ty_expr::~ty_expr() { }

	// Unit type descriptor

	unit_ty_expr::unit_ty_expr(token const& beg, token const& end)
		: ty_expr(interval(beg.Position, end.Position)) {
	}

	unit_ty_expr::~unit_ty_expr() { }

	// Identifier type descriptor

	ident_ty_expr::ident_ty_expr(token const& tok)
		: ty_expr(tok.Position), Identifier(tok.Value) {
	}

	ident_ty_expr::~ident_ty_expr() { }

	// Binary type descriptor

	bin_ty_expr::bin_ty_expr(token const& op, ysptr<ty_expr> lhs, ysptr<ty_expr> rhs)
		: ty_expr(interval(lhs->Position, rhs->Position)),
		Operator(op), LHS(lhs), RHS(rhs) {
	}

	bin_ty_expr::~bin_ty_expr() { }

	// List type descriptor

	list_ty_expr::list_ty_expr(ysptr<ty_expr>& left)
		: ty_expr(left->Position) {
		Elements.push_back(left);
	}

	list_ty_expr::~list_ty_expr() { }

	void list_ty_expr::add(ysptr<ty_expr>& exp) {
		Elements.push_back(exp);
		Position.End = exp->Position.End;
	}
}
