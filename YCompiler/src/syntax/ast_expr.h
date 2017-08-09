#pragma once

#include "ast.h"
#include "token.h"

struct AST_stmt;

enum class AST_expr_t {
	
};

struct AST_expr : public AST_node {
	AST_expr_t Ty;

	AST_expr(interval const& pos, AST_expr_t ty);
	virtual ~AST_expr();
};

