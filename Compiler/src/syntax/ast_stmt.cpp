#include "ast_stmt.h"
#include "ast_expr.h"

// AST_stmt

AST_stmt::AST_stmt(interval const& pos, AST_stmt_t ty)
	: AST_node(pos), Type(ty) {
}

AST_stmt::~AST_stmt() { }

// AST_expr_stmt

AST_expr_stmt::AST_expr_stmt(AST_expr* sub, yopt<token> semicol)
	: AST_stmt(semicol ? interval(sub->Pos, semicol->Pos) : sub->Pos
		, AST_stmt_t::Expr),
	Sub(sub) {
}

AST_expr_stmt::~AST_expr_stmt() {
	delete Sub;
}
