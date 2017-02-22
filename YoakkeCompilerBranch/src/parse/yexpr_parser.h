#pragma once

#include "prec_parser.h"

namespace yk {
	class expr;
	class expr_in_parselet;

	class yexpr_parser : public prec_parser<expr> {
	public:
		yexpr_parser(token_buffer* buff);

	public:
		void prefix(ystr const& op, ysize prec);
		void postfix(ystr const& op, ysize prec);

		void infixl(ystr const& op, ysize prec);
		void infixr(ystr const& op, ysize prec);
	};
}