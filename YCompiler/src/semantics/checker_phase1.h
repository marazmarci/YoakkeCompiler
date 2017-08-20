#pragma once

#include "checker.h"
#include "../common.h"

struct symbol_table;
struct AST_stmt;
struct AST_expr;
struct AST_ty;
struct AST_par;

namespace checker_phase1 {
	using namespace checker;

	void init(symbol_table& symt);
	result_t check(AST_stmt* st);
}
