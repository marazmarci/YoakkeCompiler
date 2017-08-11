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
