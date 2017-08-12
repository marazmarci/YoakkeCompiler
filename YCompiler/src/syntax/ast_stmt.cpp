#include "ast_stmt.h"
#include "ast_expr.h"

AST_stmt::AST_stmt(interval const& pos, AST_stmt_t ty)
	: AST_node(pos), Ty(ty) {
}

AST_stmt::~AST_stmt() { }

/*****************************************************************************/

AST_decl_stmt::AST_decl_stmt(token const& beg, token const& n, AST_expr* exp)
	: AST_stmt(interval(beg.Pos, exp->Pos), AST_stmt_t::Decl),
	Name(n), Expression(exp) {
}

AST_decl_stmt::~AST_decl_stmt() {
	delete Expression;
}

/*****************************************************************************/

AST_expr_stmt::AST_expr_stmt(AST_expr* exp)
	: AST_stmt(exp->Pos, AST_stmt_t::Expr), Expression(exp) {
}

AST_expr_stmt::AST_expr_stmt(AST_expr* exp, token const& semicol)
	: AST_stmt(interval(exp->Pos, semicol.Pos), AST_stmt_t::Expr), Expression(exp) {
}

AST_expr_stmt::~AST_expr_stmt() {
	delete Expression;
}

/*****************************************************************************/

AST_if_stmt::AST_if_stmt(token const& beg,
	AST_expr* cond, AST_block_expr* th, AST_block_expr* el)
	: AST_stmt(interval(beg.Pos, el->Pos), AST_stmt_t::If),
	Condition(cond), Then(th), Else(el) {
}

AST_if_stmt::AST_if_stmt(token const& beg, AST_expr* cond, AST_block_expr* th)
	: AST_stmt(interval(beg.Pos, th->Pos), AST_stmt_t::If),
	Condition(cond), Then(th), Else({}) {
}

AST_if_stmt::~AST_if_stmt() {
	delete Condition;
	delete Then;
	if (Else) {
		delete *Else;
	}
}
