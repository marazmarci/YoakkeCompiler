/**
 * parselet_expr.h
 * Created by: Peter Lenkefi
 * All rights reserved.
 *
 * Description:
 *  These are the parselets for expressions in the language.
 */

#pragma once

#include "token.h"
#include "ast_expr.h"

struct parser;

namespace parselet_expr {
	AST_expr* get_expr(parser& p, ysize prec = 0);
	AST_body_expr* get_body(parser& p);
	yopt<param_t> get_param(parser& p);
	AST_expr* get_func(parser& p, token const& beg);
}
