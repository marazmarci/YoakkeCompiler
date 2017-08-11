#pragma once

#include "combinator.h"
#include "../common.h"

struct AST_expr;
struct AST_stmt;
struct AST_ty;
struct AST_fn_expr;
struct AST_block_expr;
struct AST_decl_stmt;

namespace parser {
	using namespace combinator;

	result_t<AST_ty*> parse_type(token_input& in);
	result_t<AST_fn_expr*> parse_fn_expr(token_input& in);
	result_t<AST_block_expr*> parse_block_expr(token_input& in);
	result_t<AST_stmt*> parse_stmt(token_input& in);
	result_t<AST_expr*> parse_expr(token_input& in);
	result_t<AST_decl_stmt*> parse_decl_stmt(token_input& in);
}
