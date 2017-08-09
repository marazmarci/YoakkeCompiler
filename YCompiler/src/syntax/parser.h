#pragma once

#include "combinator.h"

struct AST_expr;
struct AST_stmt;
struct AST_ty;

namespace parser {
	using namespace combinator;

	result_t<token> parse_eof(token_input& in);
	result_t<AST_ty*> parse_type(token_input& in);
	result_t<AST_expr*> parse_fn_expr(token_input& in);
}
