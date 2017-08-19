#pragma once

struct AST_stmt;
struct AST_expr;
struct AST_ty;
struct AST_pat;

struct type;

namespace checker {
	void init();

	void pre_check(AST_stmt* stmt);
	yopt<type*> pre_check(AST_expr* expr);

	void check(AST_stmt* stmt);
	type* check(AST_expr* expr);
	type* check(AST_ty* ty);
	void check(AST_pat* pat);
}
