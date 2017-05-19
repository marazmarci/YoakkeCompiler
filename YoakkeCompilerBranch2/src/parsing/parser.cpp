#include "parser.h"
#include "../lexing/token.h"

namespace yk {
	parser::parser(lexer& lex, yvec<token>& buff)
		: m_Lexer(lex), m_Buffer(buff) {
	}

	token const& parser::peek(ysize delta) {
		while (delta >= m_Buffer.size()) {
			m_Buffer.push_back(m_Lexer.next());
		}
		return m_Buffer[delta];
	}

	token const& parser::last() const {
		return m_Last;
	}
	
	token parser::consume() {
		auto tok = peek();
		m_Buffer.erase(m_Buffer.begin());
		m_Last = tok;
		return tok;
	}

	yopt<token> parser::match(ytoken_t tt) {
		auto lookahead = peek();
		if (lookahead.Type == tt) {
			lookahead = consume();
			return lookahead;
		}
		return {};
	}

	yopt<ypair<token, token>> parser::match(ytoken_t tt1, ytoken_t tt2) {
		auto lookahead1 = peek();
		auto lookahead2 = peek(1);
		if (lookahead1.Type == tt1 && lookahead2.Type == tt2) {
			lookahead1 = consume();
			lookahead2 = consume();
			return { lookahead1, lookahead2 };
		}
		return {};
	}

	bool parser::same(ytoken_t tt) {
		return peek().Type == tt;
	}

	bool parser::same(ytoken_t tt1, ytoken_t tt2) {
		return peek().Type == tt1 && peek(1).Type == tt2;
	}

	parser_state parser::get_state() const {
		return std::make_pair(m_Lexer.get_state(), m_Buffer);
	}

	void parser::load_state(parser_state const& st) {
		m_Lexer.set_state(st.first);
		m_Buffer = st.second;
	}
}
