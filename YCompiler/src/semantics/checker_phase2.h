#pragma once

#include "checker.h"
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

namespace checker_phase2 {
	using namespace checker;

	using res2_t = yresult<type*, result_t>;

	void init(symbol_table& symt);
	result_t check(AST_stmt* st);
	res2_t check(AST_expr* ex);
	res2_t check(AST_ty* t);
}
