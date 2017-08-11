#pragma once

#include "ast.h"
#include "token.h"

struct AST_expr;

enum class AST_stmt_t {
	Decl,
};

struct AST_stmt : public AST_node {
	AST_stmt_t Ty;

	AST_stmt(interval const& pos, AST_stmt_t ty);
	virtual ~AST_stmt();
};

struct AST_decl_stmt : public AST_stmt {
	token		Name;
	AST_expr*	Expression;

	AST_decl_stmt(token const& beg, token const& n, AST_expr* exp);
	virtual ~AST_decl_stmt();
};
