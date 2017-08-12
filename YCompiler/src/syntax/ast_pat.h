#pragma once

#include "ast.h"
#include "token.h"

enum class AST_pat_t {
	Ident
};

struct AST_pat : public AST_node {
	AST_pat_t Ty;

	virtual ~AST_pat();

protected:
	AST_pat(interval const& pos, AST_pat_t ty);
};

struct AST_ident_pat : public AST_pat {
	ystr Value;

	AST_ident_pat(token const& val);
	virtual ~AST_ident_pat();
};
