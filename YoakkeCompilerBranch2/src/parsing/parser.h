#pragma once

#include "../common.h"
#include "../lexing/lexer.h"
#include "../lexing/token.h"

namespace yk {
	using parser_state = ypair<lexer_state, yvec<token>>;

	class parser {
	private:
		lexer&			m_Lexer;
		yvec<token>&	m_Buffer;
		token			m_Last;

	public:
		parser(lexer& lex, yvec<token>& buff);

	public:
		token const& peek(ysize delta = 0);
		token const& last() const;
		token consume();
		yopt<token> match(ytoken_t tt);
		yopt<ypair<token, token>> match(ytoken_t tt1, ytoken_t tt2);
		bool same(ytoken_t tt);
		bool same(ytoken_t tt1, ytoken_t tt2);

		parser_state get_state() const;
		void load_state(parser_state const& st);
	};
}
