#include "ast_expr.h"
#include "ast_stmt.h"
#include "ast_ty.h"
#include "ast_pat.h"

AST_expr::AST_expr(interval const& pos, AST_expr_t ty)
	: AST_node(pos), Ty(ty) {
}

AST_expr::~AST_expr() { }

/*****************************************************************************/

AST_block_expr::AST_block_expr(token const& beg,
	yvec<AST_stmt*> const& stmts, yopt<AST_expr*> val,
	token const& end) 
	: AST_expr(interval(beg.Pos, end.Pos), AST_expr_t::Block),
	Statements(stmts), Value(std::move(val)) {
}

AST_block_expr::AST_block_expr(AST_stmt* st)
	: AST_expr(st->Pos, AST_expr_t::Block),
	Statements(yvec<AST_stmt*>{ st }), Value(yopt<AST_expr*>{}) {
}

AST_block_expr::~AST_block_expr() {
	for (auto& st : Statements) {
		delete st;
	}
	if (Value) {
		delete *Value;
	}
}

/*****************************************************************************/

AST_fn_expr::AST_fn_expr(yopt<token> beg, yvec<param_t> const& params,
	yopt<AST_ty*> ret, AST_block_expr* body)
	: AST_expr(
		beg ? interval(beg->Pos, body->Pos) : body->Pos,
		AST_expr_t::Fn),
	Params(params), Return(std::move(ret)), Body(body) {
}

AST_fn_expr::~AST_fn_expr() {
	for (auto& par : Params) {
		delete std::get<1>(par);
	}
	if (Return) {
		delete *Return;
	}
	delete Body;
}

/*****************************************************************************/

AST_ident_expr::AST_ident_expr(token const& val)
	: AST_expr(val.Pos, AST_expr_t::Ident), Value(val.Value) {
}

AST_ident_expr::~AST_ident_expr() { }

/*****************************************************************************/

AST_let_expr::AST_let_expr(token const& beg,
	AST_pat* pat, yopt<AST_ty*> ty, yopt<AST_expr*> val)
	: AST_expr(interval(beg.Pos, val ? (*val)->Pos : (ty ? (*ty)->Pos : pat->Pos)),
		AST_expr_t::Let), 
	Pattern(pat), Type(std::move(ty)), Value(std::move(val)) {
}

AST_let_expr::~AST_let_expr() {
	delete Pattern;
	if (Type) {
		delete *Type;
	}
	if (Value) {
		delete *Value;
	}
}

/*****************************************************************************/

AST_if_expr::AST_if_expr(token const& beg,
	AST_expr* cond, AST_block_expr* th, yopt<AST_block_expr*> el)
	: AST_expr(interval(beg.Pos, (el ? (*el)->Pos : th->Pos)), AST_expr_t::If),
	AsStatement(false), Condition(cond), Then(th), Else(std::move(el)) {
}

AST_if_expr::~AST_if_expr() {
	delete Condition;
	delete Then;
	if (Else) {
		delete *Else;
	}
}

/*****************************************************************************/

AST_pre_expr::AST_pre_expr(token const& op, AST_expr* sub)
	: AST_expr(interval(op.Pos, sub->Pos), AST_expr_t::Pre), 
	Oper(op), Subexpr(sub) {
}

AST_pre_expr::~AST_pre_expr() {
	delete Subexpr;
}

/*****************************************************************************/

AST_bin_expr::AST_bin_expr(AST_expr* left, token const& op, AST_expr* right)
	: AST_expr(interval(left->Pos, right->Pos), AST_expr_t::Bin),
	Left(left), Oper(op), Right(right) {
}

AST_bin_expr::~AST_bin_expr() {
	delete Left;
	delete Right;
}

/*****************************************************************************/

AST_post_expr::AST_post_expr(AST_expr* sub, token const& op)
	: AST_expr(interval(sub->Pos, op.Pos), AST_expr_t::Post),
	Subexpr(sub), Oper(op) {
}

AST_post_expr::~AST_post_expr() {
	delete Subexpr;
}

/*****************************************************************************/

AST_call_expr::AST_call_expr(AST_expr* fn, yvec<AST_expr*> const& params,
	token const& end)
	: AST_expr(interval(fn->Pos, end.Pos), AST_expr_t::Call),
	Func(fn), Params(params) {
}

AST_call_expr::~AST_call_expr() {
	delete Func;
	for (auto& par : Params) {
		delete par;
	}
}
