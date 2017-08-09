#include "ast_expr.h"

AST_expr::AST_expr(interval const& pos, AST_expr_t ty)
	: AST_node(pos), Ty(ty) {
}

AST_expr::~AST_expr() { }

/*****************************************************************************/


