#include "parser.h"

namespace yk {
	parser::parser(token_buffer* buff)
		: m_Buffer(buff) {
	}

	token const& parser::peek(ysize delta) {
		while (m_Buffer->m_Buffer.size() <= delta) {
			m_Buffer->m_Buffer.push_back(m_Buffer->m_Iter->next());
		}
		return m_Buffer->m_Buffer[delta];
	}

	token parser::consume() {
		auto tok = peek();
		m_Buffer->m_Buffer.erase(m_Buffer->m_Buffer.begin());
		return tok;
	}

	yopt<token> parser::match_val(ystr const& val) {
		auto tok = peek();
		if (tok.value() == val) {
			consume();
			return tok;
		}
		else {
			return None;
		}
	}

	yopt<token> parser::match_id(ystr const& val) {
		auto tok = peek();
		if (tok.identifier() == val) {
			consume();
			return tok;
		}
		else {
			return None;
		}
	}
}