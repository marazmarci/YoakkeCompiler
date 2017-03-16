#pragma once

#include "../common.h"
#include "../lexing/lexer.h"

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

		parser_state get_state();
		void load_state(parser_state const& st);
	};
}
