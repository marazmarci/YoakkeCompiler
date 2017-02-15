#include "lexer.h"

namespace yk {
	void lexer::set_source(const char* src) {
		m_Src = src;
	}

	void lexer::add_skip(lex_rule* rule) {
		m_Skips.push_back(rule);
	}

	void lexer::add_rule(lex_rule* rule, ystr const& tok) {
		m_Rules.push_back(std::make_pair(rule, tok));
	}

	token lexer::next() {
	begin:
		for (auto& r : m_Skips) {
			if (auto res = r->match(m_Src)) {
				skip(res);
				goto begin;
			}
		}

		if (!(*m_Src)) return token("<eof>", "");

		ystr longest = "";
		ystr longest_id = "";
		for (auto& trip : m_Rules) {
			auto& rule = std::get<0>(trip);
			auto& id = std::get<1>(trip);
			if (auto res = rule->match(m_Src)) {
				ystr val = ystr(m_Src, m_Src + res);
				if (val.size() > longest.size()) {
					longest = val;
					longest_id = id;
				}
			}
		}
		if (longest.size()) {
			skip(longest.size());
			return token(longest_id, longest);
		}

		ystr err = "Unhandled token: '" + (*m_Src) + ystr("'!");
		throw std::exception(err.c_str());
	}

	bool lexer::has_next() {
		return *m_Src != '\0';
	}

	void lexer::skip(ysize delta) {
		m_Src += delta;
	}
}