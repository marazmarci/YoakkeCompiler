#pragma once

struct AST_stmt;
struct AST_expr;
struct AST_ty;

namespace AST_printer {
	void print(AST_stmt* stmt);
	void print(AST_expr* expr);
	void print(AST_ty* ty);
}
