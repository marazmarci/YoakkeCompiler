#pragma once

#include "../common.h"

struct symbol_table;
struct AST_stmt;
struct AST_expr;
struct AST_ty;
struct AST_par;

namespace checker_phase1 {
	struct semantic_err;

	using result_t = yopt<semantic_err>;

	void init(symbol_table& symt);
	void check(AST_stmt* st);
}
