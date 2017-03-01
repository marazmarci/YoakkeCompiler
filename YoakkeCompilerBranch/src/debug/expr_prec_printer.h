#pragma once

#include "../ast/expr.h"
#include "../utility/visitor.h"
#include "../common.h"

namespace yk {
	class expr_prec_printer : public visitor<expr, ystr> {
	public:
		META_Visitor(expr_prec_printer, print)

	public:
		expr_prec_printer();

	public:
		ystr print(expr&			n);
		ystr print(ident_expr&		ie);
		ystr print(bin_expr&		be);
		ystr print(preury_expr&		be);
		ystr print(postury_expr&	be);
		ystr print(list_expr&		be);
		ystr print(mixfix_expr&		be);
	};
}
