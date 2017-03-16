#include "parser.h"
#include "../lexing/token.h"

namespace yk {
	parser::parser(lexer& lex, yvec<token>& buff)
		: m_Lexer(lex), m_Buffer(buff) {
	}

	token const& parser::peek(ysize delta = 0) {
		while (delta >= m_Buffer.size()) {
			m_Buffer.push_back(m_Lexer.next());
		}
		return m_Buffer[delta];
	}
	
	token parser::consume() {
		auto tok = peek();
		m_Buffer.erase(m_Buffer.begin());
		return tok;
	}

	parser_state parser::get_state() {
		return std::make_pair(m_Lexer.get_state(), m_Buffer);
	}

	void parser::load_state(parser_state const& st) {
		m_Lexer.set_state(st.first);
		m_Buffer = st.second;
	}
}
