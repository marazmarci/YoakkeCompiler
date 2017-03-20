#pragma once

#include "../common.h"
#include "../lexing/lexer.h"
#include "../lexing/token.h"

namespace yk {
	using parser_state = ypair<lexer_state, yvec<token>>;

	class parser {
	private:
		lexer& m_Lexer;
		yvec<token>& m_Buffer;

	public:
		parser(lexer& lex, yvec<token>& buff);

	public:
		token const& peek(ysize delta = 0);
		token consume();

		template <typename T>
		yopt<token> match(T tt) {
			auto lookahead = peek();
			if (lookahead.Type == static_cast<token::type_t>(tt)) {
				lookahead = consume();
				return lookahead;
			}
			return {};
		}

		parser_state get_state();
		void load_state(parser_state const& st);
	};
}
