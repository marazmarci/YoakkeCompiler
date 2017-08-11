#pragma once

#include "ast.h"
#include "token.h"

struct AST_stmt;
struct AST_ty;

enum class AST_expr_t {
	Block, 
	Fn,
};

struct AST_expr : public AST_node {
	AST_expr_t Ty;

	AST_expr(interval const& pos, AST_expr_t ty);
	virtual ~AST_expr();
};

struct AST_block_expr : public AST_expr {
	yvec<AST_stmt*> Statements;
	yopt<AST_expr*> Value;

	AST_block_expr(token const& beg, 
		yvec<AST_stmt*> const& stmts, yopt<AST_expr*> val, 
		token const& end);
	virtual ~AST_block_expr();
};

struct AST_fn_expr : public AST_expr {
	using param_t = ytup<yopt<token>, AST_ty*>;

	yvec<param_t> Params;
	yopt<AST_ty*> Return;
	AST_block_expr* Body;

	AST_fn_expr(yopt<token> beg, yvec<param_t> const& params, 
		yopt<AST_ty*> ret, AST_block_expr* body);
	virtual ~AST_fn_expr();
};
