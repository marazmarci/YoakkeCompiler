#pragma once

struct AST_stmt;
struct AST_expr;
struct AST_ty;
struct AST_pat;

namespace AST_printer {
	void print(AST_stmt* stmt);
	void print(AST_expr* expr);
	void print(AST_ty* ty);
	void print(AST_pat* pat);
}
