#include "ast_stmt.h"

AST_stmt::AST_stmt(interval const& pos, AST_stmt_t ty)
	: AST_node(pos), Ty(ty) {
}

AST_stmt::~AST_stmt() { }

/*****************************************************************************/

