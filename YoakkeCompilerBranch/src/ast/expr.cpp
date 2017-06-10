#include "expr.h"
#include "type_desc.h"
#include "pattern.h"

namespace yk {
	// Expression
	expr::expr(point const& pos)
		: ast_node(pos), Hint(nullptr), EvalType(nullptr), Lvalue(false) {
	}

	expr::~expr() { }

	// Identifier expression
	ident_expr::ident_expr(token const& tok)
		: expr(point::get(tok)), identifier(tok.value()), ValueSymbol(nullptr) {
	}

	ident_expr::~ident_expr() { }

	// Unit expression
	unit_expr::unit_expr(token const& beg, token const& end)
		: expr(point::interval(point::get(beg), point::get(end))) {
	}

	unit_expr::~unit_expr() { }

	// Integer literal expression
	int_lit_expr::int_lit_expr(token const& tok)
		: expr(point::get(tok)), Value(std::atoi(tok.value().c_str())) {
	}

	int_lit_expr::~int_lit_expr() { }

	// Real literal expression
	real_lit_expr::real_lit_expr(token const& tok)
		: expr(point::get(tok)), Value(std::atof(tok.value().c_str())) {
	}

	real_lit_expr::~real_lit_expr() { }

	// Binary expression
	bin_expr::bin_expr(expr* l, expr* r, token const& o)
		: expr(point::interval(l->Position, r->Position)), LHS(l), RHS(r), OP(o),
		Function(nullptr) {
	}

	bin_expr::~bin_expr() {
		delete LHS;
		delete RHS;
	}

	// Prefix unary expression
	preury_expr::preury_expr(expr* s, token const& o)
		: expr(point::interval(point::get(o), s->Position)), Sub(s), OP(o) {
	}

	preury_expr::~preury_expr() {
		delete Sub;
	}

	// Postfix unary expression
	postury_expr::postury_expr(expr* s, token const& o)
		: expr(point::interval(s->Position, point::get(o))), Sub(s), OP(o) {
	}

	postury_expr::~postury_expr() {
		delete Sub;
	}

	// Expression list expression
	list_expr::list_expr(yvec<expr*> const& ls)
		: expr(point::interval(ls[0]->Position, ls[ls.size() - 1]->Position)), List(ls) {
	}

	list_expr::~list_expr() {
		for (auto i : List) delete i;
	}

	// Mixfix expression
	mixfix_expr::mixfix_expr(ystr const& o, yvec<expr*> const& opers, point const& pos)
		: expr(pos), OP(o), Operands(opers) {
	}

	mixfix_expr::~mixfix_expr() {
		for (auto o : Operands) delete o;
	}

	// Block expression
	block_expr::block_expr(point const& pos)
		: expr(pos) {
	}

	block_expr::~block_expr() { }

	// Function prototype
	func_proto::func_proto(token const& beg, point const& end, 
		yvec<param_expr*> const& pars, ty_expr* rett,
		ymap<ystr, node_tag*> const& tags)
		: expr(point::interval(point::get(beg), end)),
		Parameters(pars), ReturnType(rett), Tags(tags) {
	}

	func_proto::~func_proto() {
		delete ReturnType;
		for (auto p : Parameters) delete p;
	}

	// Function expression
	func_expr::func_expr(func_proto* pr, expr* bod)
		: block_expr(point::interval(pr->Position, bod->Position)), Prototype(pr), Body(bod) {
	}

	func_expr::~func_expr() {
		delete Prototype;
		delete Body;
	}

	// Block expression
	body_expr::body_expr(token const& beg, token const& end, yvec<stmt*> const& stmts)
		: block_expr(point::interval(point::get(beg), point::get(end))),
		Statements(stmts) {
	}

	body_expr::~body_expr() { }

	// Parameter expr
	param_expr::param_expr(token const& name, token const& col, ty_expr* type)
		: expr(point::interval(point::get(name), type->Position)),
		Name(name), Type(type) {
	}

	param_expr::param_expr(token const& col, ty_expr* type)
		: expr(point::interval(point::get(col), type->Position)),
		Name(None), Type(type), ValueSymbol(nullptr) {
	}

	param_expr::~param_expr() {
		delete Type;
	}

	// Let expression
	let_expr::let_expr(token const& beg, pat_expr* l, ty_expr* t, expr* v)
		: expr(point::interval(point::get(beg),
			v ? v->Position : (t ? t->Position : l->Position))),
			Left(l), Type(t), Value(v) {
	}

	let_expr::~let_expr() {
		delete Left;
		if (Type) delete Type;
		if (Value) delete Value;
	}

	// Const assignment expression
	const_asgn_expr::const_asgn_expr(ident_expr* l, expr* r)
		: expr(point::interval(l->Position, r->Position)),
		LHS(l), RHS(r) {
	}

	const_asgn_expr::~const_asgn_expr() {
		delete LHS;
		delete RHS;
	}
}