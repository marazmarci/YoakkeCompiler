#pragma once

#include "ast.h"
#include "token.h"

struct AST_expr;
struct AST_ty;
struct AST_fn_expr;
struct type;

enum class AST_stmt_t {
	FnDecl,
	OpDecl,
	ConstDecl,
	TyDecl,
	Expr,
	DbgWriteTy,
};

struct AST_stmt : public AST_node {
	AST_stmt_t Ty;

	virtual ~AST_stmt();

protected:
	AST_stmt(interval const& pos, AST_stmt_t ty);
};

struct AST_const_decl_stmt : public AST_stmt {
	token		Name;
	AST_expr*	Expression;

	AST_const_decl_stmt(token const& beg, token const& n, AST_expr* exp);
	virtual ~AST_const_decl_stmt();
};

struct AST_fn_decl_stmt : public AST_stmt {
	token			Name;
	AST_fn_expr*	Expression;

	AST_fn_decl_stmt(token const& beg, token const& n, AST_fn_expr* exp);
	virtual ~AST_fn_decl_stmt();
};

struct AST_op_decl_stmt : public AST_stmt {
	ystr			Operator;
	AST_fn_expr*	Expression;

	AST_op_decl_stmt(token const& beg, ystr const& op, AST_fn_expr* exp);
	virtual ~AST_op_decl_stmt();
};

struct AST_ty_decl_stmt : public AST_stmt {
	token		Name;
	AST_ty*		Type;
	type*		Symbol;

	AST_ty_decl_stmt(token const& beg, token const& n, AST_ty* ty);
	virtual ~AST_ty_decl_stmt();
};

struct AST_expr_stmt : public AST_stmt {
	AST_expr* Expression;

	AST_expr_stmt(AST_expr* exp);
	AST_expr_stmt(AST_expr* exp, token const& semicol);
	virtual ~AST_expr_stmt();
};

struct AST_dbg_write_ty_stmt : public AST_stmt {
	AST_expr* Expression;

	AST_dbg_write_ty_stmt(token const& beg, AST_expr* exp);
	virtual ~AST_dbg_write_ty_stmt();
};
