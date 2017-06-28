#include "ast_expr.h"
#include "ast_stmt.h"
#include "ast_ty.h"

// AST_expr

AST_expr::AST_expr(interval const& pos, AST_expr_t ty)
	: AST_node(pos), Type(ty) {
}

AST_expr::~AST_expr() { }

bool AST_expr::is_braced() const {
	return Type == AST_expr_t::Func;
}

// AST_ident_expr

AST_ident_expr::AST_ident_expr(token const& tok)
	: AST_expr(tok.Pos, AST_expr_t::Ident), ID(tok.Value) {
}

AST_ident_expr::~AST_ident_expr() { }

// AST_int_lit_expr

AST_int_lit_expr::AST_int_lit_expr(token const& tok)
	: AST_expr(tok.Pos, AST_expr_t::IntLit),
	Value(std::atoll(tok.Value.c_str())) {
}

AST_int_lit_expr::~AST_int_lit_expr() { }

// AST_pre_expr

AST_pre_expr::AST_pre_expr(token const& op, AST_expr* sub)
	: AST_expr(interval(op.Pos, sub->Pos), AST_expr_t::PreOp),
	OP(op), Sub(sub) {
}

AST_pre_expr::~AST_pre_expr() {
	delete Sub;
}

// AST_post_expr

AST_post_expr::AST_post_expr(token const& op, AST_expr* sub)
	: AST_expr(interval(sub->Pos, op.Pos), AST_expr_t::PostOp),
	OP(op), Sub(sub) {
}

AST_post_expr::~AST_post_expr() {
	delete Sub;
}

// AST_bin_expr

AST_bin_expr::AST_bin_expr(token const& op, AST_expr* left, AST_expr* right)
	: AST_expr(interval(left->Pos, right->Pos), AST_expr_t::BinOp),
	OP(op), LHS(left), RHS(right) {
}

AST_bin_expr::~AST_bin_expr() {
	delete LHS;
	delete RHS;
}

// AST_body_expr

AST_body_expr::AST_body_expr(token const& beg, token const& end,
	yvec<AST_stmt*> const& stmts, AST_expr* ret)
	: Pos(beg.Pos, end.Pos), Statements(stmts), Return(ret) {
}

AST_body_expr::AST_body_expr(AST_stmt* stmt)
	: Pos(stmt->Pos), Statements{ stmt }, Return(nullptr) {
}

AST_body_expr::~AST_body_expr() {
	for (auto s : Statements) {
		delete s;
	}
	delete Return;
}

// AST_func_expr

AST_func_expr::AST_func_expr(
	token const& beg,
	yopt<token> name,
	yvec<param_t> const& args, AST_ty* ret,
	AST_body_expr* body)
	: AST_expr(interval(beg.Pos, body->Pos), AST_expr_t::Func),
	Args(args), Return(ret), Body(body), Label(name) {
}

AST_func_expr::~AST_func_expr() {
	for (auto a : Args) {
		delete a.second;
	}
	delete Return;
	delete Body;
}

// AST_call_expr

AST_call_expr::AST_call_expr(
	AST_expr* fn, yvec<AST_expr*> const& args, token const& end)
	: AST_expr(interval(fn->Pos, end.Pos), AST_expr_t::Call),
	Func(fn), Args(args) {
}

AST_call_expr::~AST_call_expr() {
	delete Func;
	for (auto a : Args) {
		delete a;
	}
}

// AST_if_expr

AST_if_expr::AST_if_expr(
	token const& beg,
	yopt<token> name,
	AST_expr* cond, AST_ty* ret,
	AST_body_expr* then, AST_body_expr* els)
	: AST_expr(interval(beg.Pos, els ? els->Pos : then->Pos), AST_expr_t::If),
	Label(name), Condition(cond), Return(ret),
	Then(then), Else(els) {
}

AST_if_expr::~AST_if_expr() {
	delete Condition;
	delete Return;
	delete Then;
	delete Else;
}

// AST_list_expr

AST_list_expr::AST_list_expr(yvec<AST_expr*> const& elems)
	: AST_expr(interval((*elems.begin())->Pos, 
	(*elems.rbegin())->Pos), 
	AST_expr_t::List),
	Elements(elems) {
}

AST_list_expr::~AST_list_expr() {
	for (auto el : Elements) {
		delete el;
	}
}
