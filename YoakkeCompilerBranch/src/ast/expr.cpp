#include "expr.h"
#include "type_desc.h"

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

	// Unit expression
	unit_expr::unit_expr(token const& beg, token const& end)
		: expr(position::interval(position::get(beg), position::get(end))) {
	}

	unit_expr::~unit_expr() { }

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

	// Mixfix expression
	mixfix_expr::mixfix_expr(ystr const& o, yvec<expr*> const& opers, position const& pos)
		: expr(pos), OP(o), Operands(opers) {
	}

	mixfix_expr::~mixfix_expr() {
		for (auto o : Operands) delete o;
	}

	// Block expression
	block_expr::block_expr(position const& pos)
		: expr(pos) {
	}

	block_expr::~block_expr() { }

	// Function prototype
	func_proto::func_proto(token const& beg, position const& end, 
		yvec<param_expr*> const& pars, type_desc* rett)
		: block_expr(position::interval(position::get(beg), end)),
		Parameters(pars), ReturnType(rett) {
	}

	func_proto::~func_proto() {
		delete ReturnType;
		for (auto p : Parameters) delete p;
	}

	// Function expression
	func_expr::func_expr(func_proto* pr, expr* bod)
		: block_expr(position::interval(pr->Position, bod->Position)), Prototype(pr), Body(bod) {
	}

	func_expr::~func_expr() {
		delete Prototype;
		delete Body;
	}

	// Block expression
	body_expr::body_expr(token const& beg, token const& end) 
		: block_expr(position::interval(position::get(beg), position::get(end))) {
	}

	body_expr::~body_expr() { }

	// Parameter expr
	param_expr::param_expr(token const& name, token const& col, type_desc* type)
		: expr(position::interval(position::get(name), type->Position)),
		Name(name), Type(type) {
	}

	param_expr::param_expr(token const& col, type_desc* type)
		: expr(position::interval(position::get(col), type->Position)),
		Name(None), Type(type) {
	}

	param_expr::~param_expr() {
		delete Type;
	}
}