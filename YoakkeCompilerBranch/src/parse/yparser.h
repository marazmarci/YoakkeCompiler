#pragma once

#include "prec_parser.h"
#include "../ast/expr.h"
#include "../ast/type_desc.h"

namespace yk {
	class expr;
	class expr_in_parselet;

	class yparser : public prec_parser<expr>, public prec_parser<type_desc> {
	public:
		yparser(token_buffer* buff);

	public:
		expr* parse_expr();

		void prefix(ystr const& op, ysize prec);
		void postfix(ystr const& op, ysize prec);

		void infixl(ystr const& op, ysize prec);
		void infixr(ystr const& op, ysize prec);
	};
}