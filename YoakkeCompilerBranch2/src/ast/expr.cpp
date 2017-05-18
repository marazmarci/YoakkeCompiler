#include <cstdlib>
#include "ast.h"

namespace yk {
	// Abstract expression

	expr::expr(interval const& pos)
		: node(pos), HintType(nullptr) {
	}

	expr::~expr() { }

	// Braced expression (does not inherit from expression)

	braced_expr::braced_expr()
		: ReturnDest(true) {
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

	// Unary expression

	ury_expr::ury_expr(token const& op, ysptr<expr> sub, interval const& pos)
		: expr(pos), Operator(op), Sub(sub) {
	}

	ury_expr::~ury_expr() { }

	// Prefix unary expression

	preury_expr::preury_expr(token const& op, ysptr<expr> sub)
		: ury_expr(op, sub, interval(op.Position, sub->Position)) {
	}

	preury_expr::~preury_expr() { }

	// Postfix unary expression

	postury_expr::postury_expr(token const& op, ysptr<expr> sub)
		: ury_expr(op, sub, interval(sub->Position, op.Position)) {
	}

	postury_expr::~postury_expr() { }

	// Binary expression

	bin_expr::bin_expr(token const& op, ysptr<expr> lhs, ysptr<expr> rhs)
		: expr(interval(lhs->Position, rhs->Position)),
		Operator(op), LHS(lhs), RHS(rhs) {
	}

	bin_expr::~bin_expr() { }

	// Binary operation expression

	binop_expr::binop_expr(token const& op, ysptr<expr> lhs, ysptr<expr> rhs)
		: bin_expr(op, lhs, rhs) {
	}

	binop_expr::~binop_expr() { }

	// Assignment expression

	asgn_expr::asgn_expr(token const& op, ysptr<expr> lhs, ysptr<expr> rhs)
		: bin_expr(op, lhs, rhs) {
	}

	asgn_expr::~asgn_expr() { }

	// Constant assignment expression

	const_asgn_expr::const_asgn_expr(token const& op, ysptr<expr> lhs, ysptr<expr> rhs)
		: bin_expr(op, lhs, rhs) {
	}

	const_asgn_expr::~const_asgn_expr() { }

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

	fnproto_expr::fnproto_expr(token const& begpar, token const& endpar, yvec<param_t> params, ysptr<ty_expr> ret)
		: expr(interval(begpar.Position, ret ? ret->Position : endpar.Position)),
		Parameters(params), ReturnType(ret) {
	}

	fnproto_expr::~fnproto_expr() { }

	// Function expression

	fn_expr::fn_expr(token const& begpar, yvec<param_t> params, ysptr<ty_expr> ret, ysptr<block_expr> bd)
		: expr(interval(begpar.Position, bd->Position)),
		Parameters(params), ReturnType(ret), Body(bd) {
	}

	fn_expr::~fn_expr() { }

	// Let expression

	let_expr::let_expr(ysptr<pat_expr> pat, ysptr<ty_expr> ty, ysptr<expr> val, token const& beg)
		: expr(interval(beg.Position, val ? val->Position : (ty ? ty->Position : pat->Position))),
		Pattern(pat), Type(ty), Value(val) {
	}

	let_expr::~let_expr() { }
}
