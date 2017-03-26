#pragma once

#include "parser.h"
#include "prec_parser.h"
#include "../lexing/ylexer.h"

namespace yk {
	class expr;

	class yparser : public parser {
	private:
		ystr m_File;
		yvec<token> m_Buffer;
		ylexer m_Lexer;
		prec_parser<expr, yparser> m_ExprParser;

	public:
		yparser(ystr const& file);

	public:
		yshared_ptr<expr> parse_expr(ysize prec = 0);
		
		ystr const& file() const;

	private:
		template <typename T, typename... Args>
		void register_expr(ytoken_t tt, Args... args) {
			m_ExprParser.register_rule(tt, std::make_shared<T>(args...));
		}

	public:
		static ystr format_token(token const& t);
	};
}
