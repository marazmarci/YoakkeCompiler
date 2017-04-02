#include <cstdlib>
#include "expr.h"
#include "type_desc.h"

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
		Elements.push_back(left);
	}

	list_expr::~list_expr() { }

	void list_expr::add(yshared_ptr<expr>& exp) {
		Elements.push_back(exp);
		Position.End = exp->Position.End;
	}

	// Function call expression

	call_expr::call_expr(yshared_ptr<expr>& fn, yshared_ptr<expr>& args, token const& end)
		: expr(interval(fn->Position, end.Position)), Function(fn), Args(args) {
	}

	call_expr::~call_expr() { }

	// Block expression

	block_expr::block_expr(yvec<yshared_ptr<stmt>> sts, token const& beg, token const& end)
		: expr(interval(beg.Position, end.Position)), Statements(sts) {
	}

	block_expr::~block_expr() { }

	// Function prototype

	fnproto_expr::fnproto_expr(yvec<param_t> params, yshared_ptr<type_desc> ret, token const& begpar, token const& endpar)
		: expr(interval(begpar.Position, ret ? ret->Position : endpar.Position)),
		Parameters(params), ReturnType(ret) {
	}

	fnproto_expr::~fnproto_expr() { }

	// Function expression

	fn_expr::fn_expr(yshared_ptr<fnproto_expr> proto, yshared_ptr<block_expr> bd)
		: expr(interval(proto->Position, bd->Position)),
		Prototype(proto), Body(bd) {
	}

	fn_expr::~fn_expr() { }
}
