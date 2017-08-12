#pragma once

#include "ast.h"
#include "token.h"

struct AST_expr;

enum class AST_stmt_t {
	Decl,
	Expr,
	If,
};

struct AST_stmt : public AST_node {
	AST_stmt_t Ty;

	virtual ~AST_stmt();

protected:
	AST_stmt(interval const& pos, AST_stmt_t ty);
};

struct AST_decl_stmt : public AST_stmt {
	token		Name;
	AST_expr*	Expression;

	AST_decl_stmt(token const& beg, token const& n, AST_expr* exp);
	virtual ~AST_decl_stmt();
};

struct AST_expr_stmt : public AST_stmt {
	AST_expr* Expression;

	AST_expr_stmt(AST_expr* exp);
	AST_expr_stmt(AST_expr* exp, token const& semicol);
	virtual ~AST_expr_stmt();
};

struct AST_if_stmt : public AST_stmt {
	AST_expr* Condition;
	AST_block_expr* Then;
	yopt<AST_block_expr*> Else;

	AST_if_stmt(token const& beg, AST_expr* cond, AST_block_expr* th, AST_block_expr* el);
	AST_if_stmt(token const& beg, AST_expr* cond, AST_block_expr* th);
	virtual ~AST_if_stmt();
};
