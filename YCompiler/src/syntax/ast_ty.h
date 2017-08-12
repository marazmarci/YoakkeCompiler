#pragma once

#include "ast.h"
#include "token.h"

enum class AST_ty_t {
	Ident
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
