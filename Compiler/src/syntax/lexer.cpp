#include <cassert>
#include <cctype>
#include <cstring>
#include "lexer.h"

lexer::lexer(file_hnd const& src) 
	: m_File(src), m_State(0, 0), m_Last(0, 0) {
	// The pointer is either the first char or null if there are no lines
	m_Ptr = m_File.line_count() ? m_File.line(0).first : nullptr;

	// Initialize symbols
	add_symbol("(",		token_t::LParen		);	// '('
	add_symbol(")",		token_t::RParen		);	// ')'
	add_symbol("{",		token_t::LBrace		);	// '{'
	add_symbol("}",		token_t::RBrace		);	// '}'
	add_symbol("[",		token_t::LBracket	);	// '['
	add_symbol("]",		token_t::Rbracket	);	// ']'
	add_symbol("+",		token_t::Add		);	// '+'
	add_symbol("-",		token_t::Sub		);	// '-'
	add_symbol("*",		token_t::Mul		);	// '*'
	add_symbol("/",		token_t::Div		);	// '/'
	add_symbol("%",		token_t::Mod		);	// '%'
	add_symbol("=",		token_t::Asgn		);	// '='
	add_symbol("==",	token_t::Eq			);	// '=='
	add_symbol("<>",	token_t::Neq		);	// '<>'
	add_symbol(">",		token_t::Greater	);	// '>'
	add_symbol("<",		token_t::Less		);	// '<'
	add_symbol(">=",	token_t::GrEq		);	// '>='
	add_symbol("<=",	token_t::LeEq		);	// '<='
	add_symbol(".",		token_t::Dot		);	// '.'
	add_symbol(":",		token_t::Colon		);	// ':'
	add_symbol(";",		token_t::Semicol	);	// ';'
	add_symbol("->",	token_t::Arrow		);	// '->'
}

lexer_state lexer::get_state() const {
	return m_State;
}

void lexer::set_state(lexer_state const& state) {
	m_State = state;
	m_Ptr = m_File.line(state.Row).first + state.Column;
}

token lexer::next() {
	while (true) {
		if (end()) {
			return token(interval(m_State, 1), token_t::EndOfFile);
		}

		// Skip whitespace
		if (std::isspace(*m_Ptr)) {
			advance();
			continue;
		}

		// Check for comments

		// Single-line comment
		if (std::strncmp(m_Ptr, "//", 2) == 0) {
			advance(2);
			// Go until we encounter the end of file or a newline
			while (!end() && *m_Ptr != '\n') {
				advance();
			}
			continue;
		}

		// Multi-line, possibly nested comments
		if (std::strncmp(m_Ptr, "/*", 2) == 0) {
			advance(2);
			// Save where we are for error if we reach an EOF
			interval comment_beg = interval(m_State, 2);
			// Nesting depth
			ysize depth = 1;
			while (depth) {
				if (std::strncmp(m_Ptr, "/*", 2) == 0) {
					// One more nesting
					advance(2);
					depth++;
				}
				else if (std::strncmp(m_Ptr, "*/", 2) == 0) {
					// A nesting got closed
					advance(2);
					depth--;
				}
				else if (end()) {
					throw lexer_eof_exception(m_File,
						comment_beg, interval(m_Last, 1),
						"A nested comment was unescaped before the end-of-file!",
						"Yoakke supports nested comments!");
				}
				else {
					advance();
				}
			}
			continue;
		}

		// See if we have encountered a symbol
		// We go in reverse order, so from longest to shortest so if
		// we have '<=' for example, it does not get separated into
		// '<' and '='. The map always sorts by the keys.
		for (auto it = m_Symbols.rbegin(); it != m_Symbols.rend(); it++) {
			ystr const& sym = it->first;
			if (std::strncmp(sym.c_str(), m_Ptr, sym.length()) == 0) {
				// We have found a matching symbol
				// Advance the pointer and position
				advance(sym.length());
				// Return the token with the type
				return token(interval(m_State, sym.length()), it->second, it->first);
			}
		}

		// Check if it is a number
		if (std::isdigit(*m_Ptr)) {
			ystr num = "";
			while (std::isdigit(*m_Ptr)) {
				num += *m_Ptr;
				advance();
			}
			// Check if real
			if (*m_Ptr == '.' && std::isdigit(*(m_Ptr + 1))) {
				num += '.';
				advance();
				while (std::isdigit(*m_Ptr)) {
					num += *m_Ptr;
					advance();
				}
				return token(interval(m_State, num.length()), 
					token_t::RealLit, num);
			}
			else {
				return token(interval(m_State, num.length()),
					token_t::IntLit, num);
			}
		}

		// Check if it is identifier-based
		if (std::isalpha(*m_Ptr) || *m_Ptr == '_') {
			ystr ident = "";
			while (std::isalnum(*m_Ptr) || *m_Ptr == '_') {
				ident += *m_Ptr;
				advance();
			}

			// Check if it is a registered keyword
			auto it = m_Keywords.find(ident);
			if (it == m_Keywords.end()) {
				// No such keyword, identifier
				return token(interval(m_State, ident.length()),
					token_t::Ident, ident);
			}
			else {
				// There is a keyword with a given type
				return token(interval(m_State, ident.length()),
					it->second);
			}
		}

		throw lexer_unknown_exception(m_File, 
			interval(point(m_State.Column + 1, m_State.Row), 1), 
			ystr{ *m_Ptr });
	}

	assert(false && "Unreachable!");
}

bool lexer::has_next() const {
	return !end();
}

bool lexer::end() const {
	return *m_Ptr == '\0' || *m_Ptr == EOF;
}

void lexer::advance(ysize delta) {
	for (ysize i = 0; i < delta; i++, m_Ptr++) {
		if (*m_Ptr == '\n') {
			// We have encountered a newline, go to the next line
			m_State.Column = 0;
			m_State.Row++;
		}
		else {
			// No new line, go to the next char
			m_State.Column++;
			if (!std::isspace(*m_Ptr)) {
				// This is a visible character
				m_Last = m_State;
			}
		}
	}
}

void lexer::add_symbol(ystr const& sym, token_t ty) {
	assert(m_Symbols.find(sym) == m_Symbols.end() && "Symbol already inserted!");
	m_Symbols.insert({ sym, ty });
}

void lexer::add_keyword(ystr const& kw, token_t ty) {
	assert(m_Keywords.find(kw) == m_Keywords.end() && "Keyword already inserted!");
	m_Keywords.insert({ kw, ty });
}
