#pragma once

#include "ast.h"
#include "token.h"

enum class AST_ty_t {
	Ident,
	Pre,
	Bin,
	Post,
};

struct AST_ty : public AST_node {
	AST_ty_t Ty;

	virtual ~AST_ty();

protected:
	AST_ty(interval const& pos, AST_ty_t ty);
};

struct AST_ident_ty : public AST_ty {
	ystr Value;

	AST_ident_ty(token const& val);
	virtual ~AST_ident_ty();
};

struct AST_pre_ty : public AST_ty {
	token		Oper;
	AST_ty*		Subexpr;

	AST_pre_ty(token const& op, AST_ty* sub);
	virtual ~AST_pre_ty();
};

struct AST_bin_ty : public AST_ty {
	AST_ty*	Left;
	token	Oper;
	AST_ty*	Right;

	AST_bin_ty(AST_ty* left, token const& op, AST_ty* right);
	virtual ~AST_bin_ty();
};

struct AST_post_ty : public AST_ty {
	AST_ty*		Subexpr;
	token		Oper;

	AST_post_ty(AST_ty* sub, token const& op);
	virtual ~AST_post_ty();
};
