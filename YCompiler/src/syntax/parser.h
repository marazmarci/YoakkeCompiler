#pragma once

#include "combinator.h"
#include "../common.h"

struct AST_expr;
struct AST_stmt;
struct AST_ty;
struct AST_pat;
struct AST_fn_expr;
struct AST_block_expr;
struct AST_decl_stmt;
struct AST_let_expr;
struct AST_if_expr;

namespace parser {
	using namespace combinator;

	result_t<yvec<AST_stmt*>> parse_program(token_input& in);
	result_t<AST_expr*> parse_expr(token_input& in);

	void handle_error(fail_info& err);
}
