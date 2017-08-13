#pragma once

#include "ast.h"
#include "token.h"

enum class AST_pat_t {
	Ident,
	Pre,
	Bin,
	Post,
	List,
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

struct AST_pre_pat : public AST_pat {
	token		Oper;
	AST_pat*	Subexpr;

	AST_pre_pat(token const& op, AST_pat* sub);
	virtual ~AST_pre_pat();
};

struct AST_bin_pat : public AST_pat {
	AST_pat*	Left;
	token		Oper;
	AST_pat*	Right;

	AST_bin_pat(AST_pat* left, token const& op, AST_pat* right);
	virtual ~AST_bin_pat();
};

struct AST_post_pat : public AST_pat {
	AST_pat*	Subexpr;
	token		Oper;

	AST_post_pat(AST_pat* sub, token const& op);
	virtual ~AST_post_pat();
};

struct AST_list_pat : public AST_pat {
	yvec<AST_pat*>	Elements;

	AST_list_pat(yvec<AST_pat*> const& elems);
	AST_list_pat(token const& beg, token const& end);
	virtual ~AST_list_pat();
};
