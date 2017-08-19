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
	result_t<AST_ty*> parse_type(token_input& in);
	result_t<AST_fn_expr*> parse_fn_expr(token_input& in);
	result_t<AST_block_expr*> parse_block_expr(token_input& in);
	result_t<AST_stmt*> parse_stmt(token_input& in);
	result_t<AST_expr*> parse_expr(token_input& in);
	result_t<AST_pat*> parse_pat(token_input& in);
	result_t<AST_stmt*> parse_decl_stmt(token_input& in);
	result_t<AST_let_expr*> parse_let_expr(token_input& in);
	result_t<AST_if_expr*> parse_if_expr(token_input& in);
	result_t<AST_expr*> parse_list_expr(token_input& in);

	void handle_error(fail_info& err);
}
