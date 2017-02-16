#include "lexer.h"

namespace yk {
	void lexer::set_source(const char* src) {
		m_Src = src;
	}

	void lexer::add_skip(lex_skip rule) {
		m_Skips.push_back(rule);
	}

	void lexer::add_rule(lex_rule rule) {
		m_Rules.push_back(rule);
	}

	token lexer::next() {
	begin:
		if (!(*m_Src)) return token("<eof>", "");

		for (auto& rule : m_Skips) {
			if (auto res = rule(m_Src)) {
				skip(res);
				goto begin;
			}
		}

		for (auto& rule : m_Rules) {
			auto res = rule(m_Src);
			if (res.some()) {
				auto unwrapped = res.get();
				ystr val = ystr(m_Src, m_Src + unwrapped.second);
				skip(unwrapped.second);
				return token(unwrapped.first, val);
			}
		}

		ystr err = "Unhandled token: '" + (*m_Src) + ystr("'!");
		throw std::exception(err.c_str());
		return token("", "");
	}

	bool lexer::has_next() {
		return *m_Src != '\0';
	}

	void lexer::skip(ysize delta) {
		m_Src += delta;
	}
}