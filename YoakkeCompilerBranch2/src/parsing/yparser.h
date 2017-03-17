#pragma once

#include "parser.h"
#include "prec_parser.h"
#include "../lexing/ylexer.h"

namespace yk {
	class expr;

	class yparser : public parser {
	private:
		yvec<token> m_Buffer;
		ylexer m_Lexer;
		prec_parser<expr, yparser> m_ExprParser;

	public:
		yparser(ystr const& file);

	public:
		yshared_ptr<expr> parse_expr(ysize prec = 0);
	};
}
