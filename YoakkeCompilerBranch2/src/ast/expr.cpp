#include <cstdlib>
#include "expr.h"
#include "type_desc.h"
#include "pattern.h"

namespace yk {
	// Abstract expression

	expr::expr(interval const& pos)
		: node(pos), HintType(nullptr) {
	}

	expr::~expr() { }

	// Braced expression (does not inherit from expression)

	braced_expr::braced_expr()
		: ReturnDestination(true) {
	}

	braced_expr::~braced_expr() { }

	// Identifier expression

	ident_expr::ident_expr(token const& tok)
		: expr(tok.Position), Identifier(tok.Value) {
	}
	
	ident_expr::~ident_expr() { }

	// Unit expression

	unit_expr::unit_expr(token const& beg, token const& end)
		: expr(interval(beg.Position, end.Position)) {
	}

	unit_expr::~unit_expr() { }

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

	list_expr::list_expr(ysptr<expr>& left)
		: expr(left->Position) {
		Elements.push_back(left);
	}

	list_expr::~list_expr() { }

	void list_expr::add(ysptr<expr>& exp) {
		Elements.push_back(exp);
		Position.End = exp->Position.End;
	}

	// Function call expression

	call_expr::call_expr(ysptr<expr>& fn, ysptr<expr>& args, token const& end)
		: expr(interval(fn->Position, end.Position)), Function(fn), Args(args) {
	}

	call_expr::~call_expr() { }

	// Block expression

	block_expr::block_expr(yvec<ysptr<stmt>> sts, token const& beg, token const& end)
		: expr(interval(beg.Position, end.Position)), Statements(sts) {
	}

	block_expr::~block_expr() { }

	// Function prototype

	fnproto_expr::fnproto_expr(token const& begpar, token const& endpar, yvec<param_t> params, ysptr<type_desc> ret)
		: expr(interval(begpar.Position, ret ? ret->Position : endpar.Position)),
		Parameters(params), ReturnType(ret) {
	}

	fnproto_expr::~fnproto_expr() { }

	// Function expression

	fn_expr::fn_expr(token const& begpar, yvec<param_t> params, ysptr<type_desc> ret, ysptr<block_expr> bd)
		: expr(interval(begpar.Position, bd->Position)),
		Parameters(params), ReturnType(ret), Body(bd) {
	}

	fn_expr::~fn_expr() { }

	// Let expression

	let_expr::let_expr(ysptr<pattern> pat, ysptr<type_desc> ty, ysptr<expr> val, token const& beg)
		: expr(interval(beg.Position, val ? val->Position : (ty ? ty->Position : pat->Position))),
		Pattern(pat), Type(ty), Value(val) {
	}

	let_expr::~let_expr() { }
}
