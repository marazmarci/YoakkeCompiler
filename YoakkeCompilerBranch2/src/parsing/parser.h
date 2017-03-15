#pragma once

#include "../common.h"
#include "../lexing/lexer.h"

namespace yk {
	using parser_state = ypair<lexer_state, yvec<token>>;

	class parser {
	private:
		lexer& m_Lexer;
		yvec<token>& m_Tokens;

	public:
		parser(lexer& lex, yvec<token>& buff);
	};
}
