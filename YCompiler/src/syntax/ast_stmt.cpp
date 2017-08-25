#include "ast_stmt.h"
#include "ast_expr.h"
#include "ast_ty.h"

AST_stmt::AST_stmt(interval const& pos, AST_stmt_t ty)
	: AST_node(pos), Ty(ty) {
}

AST_stmt::~AST_stmt() { }

/*****************************************************************************/

AST_fn_decl_stmt::AST_fn_decl_stmt(token const& beg, token const& n, AST_fn_expr* exp)
	: AST_stmt(interval(beg.Pos, exp->Pos), AST_stmt_t::FnDecl),
	Name(n), Expression(exp) {
}

AST_fn_decl_stmt::~AST_fn_decl_stmt() {
	delete Expression;
}

/*****************************************************************************/

AST_const_decl_stmt::AST_const_decl_stmt(token const& beg, token const& n, AST_expr* exp)
	: AST_stmt(interval(beg.Pos, exp->Pos), AST_stmt_t::ConstDecl),
	Name(n), Expression(exp) {
}

AST_const_decl_stmt::~AST_const_decl_stmt() {
	delete Expression;
}

/*****************************************************************************/

AST_ty_decl_stmt::AST_ty_decl_stmt(token const& beg, token const& n, AST_ty* ty)
	: AST_stmt(interval(beg.Pos, ty->Pos), AST_stmt_t::TyDecl),
	Name(n), Type(ty), Symbol(nullptr) {
}

AST_ty_decl_stmt::~AST_ty_decl_stmt() {
	delete Type;
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

AST_dbg_write_ty_stmt::AST_dbg_write_ty_stmt(token const& beg, AST_expr* exp)
	: AST_stmt(interval(beg.Pos, exp->Pos), AST_stmt_t::DbgWriteTy),
	Expression(exp) {
}

AST_dbg_write_ty_stmt::~AST_dbg_write_ty_stmt() {
	delete Expression;
}
