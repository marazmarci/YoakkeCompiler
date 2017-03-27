#include <cctype>
#include <cstring>
#include <fstream>

#include "ylexer.h"
#include "token.h"
#include "lex_error.h"

namespace yk {
	ylexer::ylexer(ystr const& file)
		: m_File(file), m_Position(0, 0) {
		std::ifstream stream(file);
		if (stream.good()) {
			m_Src = ystr((std::istreambuf_iterator<char>(stream)),
				(std::istreambuf_iterator<char>()));
			m_Ptr = &m_Src[0];
		}
		else {
			throw lex_no_file_err(m_File);
		}

		add_symbol("(", ytoken_t::Lpar);
		add_symbol(")", ytoken_t::Rpar);
		add_symbol("[", ytoken_t::Lsqbr);
		add_symbol("]", ytoken_t::Rsqbr);
		add_symbol("{", ytoken_t::Lbrace);
		add_symbol("}", ytoken_t::Rbrace);
		add_symbol(":", ytoken_t::Colon);
		add_symbol("->", ytoken_t::Arrow);
		add_symbol(";", ytoken_t::Semicol);

		// Operators
		add_symbol("::", ytoken_t::Dcolon);
		add_symbol(",", ytoken_t::Comma);
		add_symbol("=", ytoken_t::Asgn);
		add_symbol("||", ytoken_t::Or);
		add_symbol("&&", ytoken_t::And);
		add_symbol("==", ytoken_t::Eq);
		add_symbol("<>", ytoken_t::Neq);
		add_symbol(">", ytoken_t::Gr);
		add_symbol("<", ytoken_t::Le);
		add_symbol(">=", ytoken_t::Goe);
		add_symbol("<=", ytoken_t::Loe);
		add_symbol("+", ytoken_t::Add);
		add_symbol("-", ytoken_t::Sub);
		add_symbol("*", ytoken_t::Mul);
		add_symbol("/", ytoken_t::Div);
		add_symbol("%", ytoken_t::Mod);
		add_symbol(".", ytoken_t::Dot);
		add_symbol("!", ytoken_t::Not);
	}

	lexer_state ylexer::get_state() {
		return std::make_pair(m_Ptr, m_Position);
	}

	void ylexer::set_state(lexer_state pos) {
		m_Ptr = pos.first;
		m_Position = pos.second;
	}

	token ylexer::next() {
	begin:
		// EOF
		if (!has_next()) {
			return token(ytoken_t::Epsilon, "", m_Position);
		}
		// Whitespace
		if (std::isspace(*m_Ptr)) {
			advance();
			goto begin;
		}
		// Single-line comments
		if (std::strncmp("//", m_Ptr, 2) == 0) {
			advance(2);
			while (*m_Ptr && *m_Ptr != '\n') {
				advance();
			}
			goto begin;
		}
		// Multi-line comments
		if (std::strncmp("/*", m_Ptr, 2) == 0) {
			advance(2);
			ysize depth = 1;
			while (depth) {
				if (std::strncmp("/*", m_Ptr, 2) == 0) {
					advance(2);
					depth++;
				}
				else if (std::strncmp("*/", m_Ptr, 2) == 0) {
					advance(2);
					depth--;
				}
				else if (*m_Ptr) {
					advance();
				}
				else {
					throw unexpected_eof(m_File, m_Position, 
						"Multi-line comment must end before EOF! Use: '*/'");
				}
			}
			goto begin;
		}
		// Symbols
		for (auto it = m_Symbols.rbegin(); it != m_Symbols.rend(); it++) {
			auto& str = it->first;
			if (std::strncmp(m_Ptr, str.c_str(), str.length()) == 0) {
				advance(str.length());
				return token(it->second, str, m_Position);
			}
		}
		// Number
		{
			ystr num = "";
			bool real = false;
			while (std::isdigit(*m_Ptr)) {
				num += *m_Ptr;
				advance();
			}
			if (*m_Ptr == '.' && 
				(num.length() || std::isdigit(*(m_Ptr + 1)))) {
				real = true;
				num += '.';
				advance();
				while (std::isdigit(*m_Ptr)) {
					num += *m_Ptr;
					advance();
				}
			}
			if (num.length()) {
				return token(real ? ytoken_t::Real : ytoken_t::Integer,
					num, m_Position);
			}
		}
		// Identifier
		{
			ystr ident = "";
			if (std::isalpha(*m_Ptr) || *m_Ptr == '_') {
				ident += *m_Ptr;
				advance();
				while (std::isalnum(*m_Ptr) || *m_Ptr == '_') {
					ident += *m_Ptr;
					advance();
				}
				// Check if keyword
				if (auto kw = m_Keywords.at(ident)) {
					return token(kw.value().get(), ident, m_Position);
				}
				return token(ytoken_t::Ident, ident, m_Position);
			}
		}
		// Unknown
		throw unknown_tok_err(ystr{ *m_Ptr }, m_File, m_Position);
	}

	bool ylexer::has_next() {
		return *m_Ptr;
	}

	void ylexer::add_symbol(ystr const& val, ytoken_t tt) {
		m_Symbols.insert(std::make_pair(val, tt));
	}

	void ylexer::add_keyword(ystr const& val, ytoken_t tt) {
		m_Keywords.insert(std::make_pair(val, tt));
	}

	void ylexer::advance(ysize cnt) {
		while (cnt--) {
			char curr = *m_Ptr++;
			if (curr == '\n') {
				m_Position.Row++;
				m_Position.Col = 0;
			}
			else {
				m_Position.Col++;
			}
		}
	}
}
