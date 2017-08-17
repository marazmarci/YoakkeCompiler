#pragma once

struct AST_stmt;
struct AST_expr;
struct AST_ty;
struct AST_pat;

namespace checker {
	void check(AST_stmt* stmt);
	void check(AST_expr* expr);
	void check(AST_ty* ty);
	void check(AST_pat* pat);
}
