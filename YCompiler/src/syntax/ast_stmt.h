#pragma once

#include "ast.h"
#include "token.h"

struct AST_expr;

enum class AST_stmt_t {
	
};

struct AST_stmt : public AST_node {
	AST_stmt_t Ty;

	AST_stmt(interval const& pos, AST_stmt_t ty);
	virtual ~AST_stmt();
};
