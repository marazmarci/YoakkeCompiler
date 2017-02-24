#pragma once

#include "prec_parser.h"
#include "../ast/expr.h"
#include "../ast/type_desc.h"

namespace yk {
	class expr;
	class expr_in_parselet;

	class yparser : public parser {
	private:
		prec_parser<expr> m_ExprParser;
		prec_parser<type_desc> m_TypeParser;

	public:
		yparser(token_buffer* buff);

	public:
		void prefix(ystr const& op, ysize prec);
		void postfix(ystr const& op, ysize prec);

		void infixl(ystr const& op, ysize prec);
		void infixr(ystr const& op, ysize prec);

		expr* parse_expr();
		type_desc* parse_type();
	};
}