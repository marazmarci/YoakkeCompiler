#include "parser.h"

namespace yk {
	parse_state::parse_state(yvec<token> const& buff)
		: BufferCopy(buff) {
	}

	parse_state::~parse_state() { }

	parser::parser(token_buffer* buff, parser* par)
		: m_Buffer(buff), m_Parent(par) {
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

	parse_state parser::get_state() {
		return parse_state(m_Buffer->m_Buffer);
	}

	void parser::load_state(parse_state const& st) {

	}

	parser* parser::parent() {
		return m_Parent;
	}
}