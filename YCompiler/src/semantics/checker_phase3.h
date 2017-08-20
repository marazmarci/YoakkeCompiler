#pragma once

#include "symbol_table.h"
#include "../common.h"
#include "../syntax/ast_stmt.h"
#include "../syntax/ast_expr.h"
#include "../syntax/ast_ty.h"
#include "../syntax/ast_pat.h"

struct symbol_table;
struct AST_stmt;
struct AST_expr;
struct AST_ty;
struct AST_par;

namespace checker_phase3 {
	struct semantic_err;

	using result_t = yopt<semantic_err>;

	void init(symbol_table& symt);
	void check(AST_stmt* st);
}
