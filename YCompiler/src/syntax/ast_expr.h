#pragma once

#include "ast.h"
#include "token.h"

struct AST_stmt;
struct AST_ty;
struct AST_pat;

enum class AST_expr_t {
	Block, 
	Fn,
	Ident,
	Let,
	If,
	Pre,
	Bin,
	Post,
	Call,
	List,
	IntLit,
	RealLit,
};

struct AST_expr : public AST_node {
	AST_expr_t Ty;

	virtual ~AST_expr();

protected:
	AST_expr(interval const& pos, AST_expr_t ty);
};

struct AST_block_expr : public AST_expr {
	bool AsStatement;
	yvec<AST_stmt*> Statements;
	yopt<AST_expr*> Value;

	AST_block_expr(token const& beg, 
		yvec<AST_stmt*> const& stmts, yopt<AST_expr*> val, 
		token const& end);
	AST_block_expr(AST_stmt* st);
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

struct AST_ident_expr : public AST_expr {
	ystr Value;

	AST_ident_expr(token const& val);
	virtual ~AST_ident_expr();
};

struct AST_let_expr : public AST_expr {
	AST_pat* Pattern;
	yopt<AST_ty*> Type;
	yopt<AST_expr*> Value;

	AST_let_expr(token const& beg, AST_pat* pat, yopt<AST_ty*> ty, yopt<AST_expr*> val);
	virtual ~AST_let_expr();
};

struct AST_if_expr : public AST_expr {
	bool AsStatement;
	AST_expr* Condition;
	AST_block_expr* Then;
	yopt<AST_block_expr*> Else;

	AST_if_expr(token const& beg, 
		AST_expr* cond, AST_block_expr* th, yopt<AST_block_expr*> el);
	virtual ~AST_if_expr();
};

struct AST_pre_expr : public AST_expr {
	token		Oper;
	AST_expr*	Subexpr;

	AST_pre_expr(token const& op, AST_expr* sub);
	virtual ~AST_pre_expr();
};

struct AST_bin_expr : public AST_expr {
	AST_expr*	Left;
	token		Oper;
	AST_expr*	Right;

	AST_bin_expr(AST_expr* left, token const& op, AST_expr* right);
	virtual ~AST_bin_expr();
};

struct AST_post_expr : public AST_expr {
	AST_expr*	Subexpr;
	token		Oper;

	AST_post_expr(AST_expr* sub, token const& op);
	virtual ~AST_post_expr();
};

struct AST_call_expr : public AST_expr {
	AST_expr*		Func;
	yvec<AST_expr*>	Params;

	AST_call_expr(AST_expr* fn, yvec<AST_expr*> const& params, token const& end);
	virtual ~AST_call_expr();
};

struct AST_list_expr : public AST_expr {
	yvec<AST_expr*>	Elements;

	AST_list_expr(yvec<AST_expr*> const& elems);
	AST_list_expr(token const& beg, token const& end);
	virtual ~AST_list_expr();
};

struct AST_int_lit_expr : public AST_expr {
	long long int Value;

	AST_int_lit_expr(token const& tok);
	virtual ~AST_int_lit_expr();
};

struct AST_real_lit_expr : public AST_expr {
	long double Value;

	AST_real_lit_expr(token const& tok);
	virtual ~AST_real_lit_expr();
};
