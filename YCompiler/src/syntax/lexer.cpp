#include <cassert>
#include <cctype>
#include <cstring>
#include "lexer.h"

lexer_eof_err::lexer_eof_err(file_hnd const& file,
	interval const& start, interval const& end,
	ystr const& msg, yopt<ystr> note)
	: File(file), Start(start), End(end), Msg(msg), Note(note) {
}

lexer_eol_err::lexer_eol_err(file_hnd const& file,
	interval const& start, interval const& end,
	ystr const& msg, yopt<ystr> note)
	: File(file), Start(start), End(end), Msg(msg), Note(note) {
}

lexer_unk_tok_err::lexer_unk_tok_err(file_hnd const& file, 
	interval const& pos, ystr const& val)
	: File(file), Pos(pos), Tok(val) {
}

lexer::lexer(file_hnd const& src)
	: m_File(src), 
	m_State(std::make_tuple(point(), yvec<token>{})), 
	m_Last(0, 0) {
	// The pointer is either the first char or null if there are no lines
	m_Ptr = m_File.line_count() ? std::get<0>(m_File.line(0)) : nullptr;

	// Initialize symbols
	add_symbol("(", token_t::LParen);	// '('
	add_symbol(")", token_t::RParen);	// ')'
	add_symbol("{", token_t::LBrace);	// '{'
	add_symbol("}", token_t::RBrace);	// '}'
	add_symbol("[", token_t::LBracket);	// '['
	add_symbol("]", token_t::RBracket);	// ']'
	add_symbol("+", token_t::Add);		// '+'
	add_symbol("-", token_t::Sub);		// '-'
	add_symbol("*", token_t::Mul);		// '*'
	add_symbol("/", token_t::Div);		// '/'
	add_symbol("%", token_t::Mod);		// '%'
	add_symbol("=", token_t::Asgn);		// '='
	add_symbol("==", token_t::Eq);		// '=='
	add_symbol("<>", token_t::Neq);		// '<>'
	add_symbol(">", token_t::Greater);	// '>'
	add_symbol("<", token_t::Less);		// '<'
	add_symbol(">=", token_t::GrEq);	// '>='
	add_symbol("<=", token_t::LeEq);	// '<='
	add_symbol(".", token_t::Dot);		// '.'
	add_symbol(",", token_t::Comma);	// ','
	add_symbol(":", token_t::Colon);	// ':'
	add_symbol(";", token_t::Semicol);	// ';'
	add_symbol("#", token_t::Hashmark);	// '#'
	add_symbol("->", token_t::Arrow);	// '->'

	// Initialize keywords
	add_keyword("fn", token_t::Fn);				// 'fn'
	add_keyword("type", token_t::Type);			// 'type'
	add_keyword("const", token_t::Const);		// 'const'
	add_keyword("if", token_t::If);				// 'if'
	add_keyword("else", token_t::Else);			// 'else'
	add_keyword("let", token_t::Let);			// 'let'
	add_keyword("mut", token_t::Mut);			// 'mut'
	add_keyword("true", token_t::True);			// 'true'
	add_keyword("false", token_t::False);		// 'false'
	add_keyword("operator", token_t::Operator);	// 'operator'
	
	// Debug keyword
	add_keyword("write_ty", token_t::DbgWriteTy);	// 'write_ty'
}

file_hnd const& lexer::file() const {
	return m_File;
}

lexer_state const& lexer::get_state() const {
	return m_State;
}

lexer_state& lexer::get_state() {
	return m_State;
}

void lexer::set_state(lexer_state const& state) {
	m_State = state;
	auto& pt = std::get<0>(m_State);
	m_Ptr = std::get<0>(m_File.line(pt.Row)) + pt.Column;
}

yresult<token, lexer_err> lexer::next() {
	while (true) {
		if (end()) {
			return token(interval(m_Last, 1), token_t::EndOfFile);
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
			interval comment_beg = interval(std::get<0>(m_State), 2);
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
					return lexer_err(lexer_eof_err(
						m_File,
						comment_beg, interval(m_Last, 1),
						ystr("A nested comment was unescaped before the end-of-file!"),
						ystr("Yoakke supports nested comments!")));
				}
				else {
					advance();
				}
			}
			continue;
		}

		// String literals
		if (*m_Ptr == '"') {
			ysize str_len = 1;
			advance();
			ystr str = "";
			interval str_beg = interval(std::get<0>(m_State), 1);
			while (true) {
				if (end()) {
					return lexer_err(lexer_eof_err(
						m_File,
						str_beg, interval(m_Last, 1),
						ystr("A string literal was not ended before the end-of-file!"),
						{}));
				}
				char curr = *m_Ptr;
				if (curr == '"') {
					str_len++;
					advance();
					return token(interval(std::get<0>(m_State), str_len), token_t::StrLit, str);
				}
				else if (curr == '\r') {
					advance();
					return lexer_err(lexer_eof_err(
						m_File,
						str_beg, interval(m_Last, 1),
						ystr("A string literal was not ended before the end-of-line!"),
						ystr("Yoakke does not support multi-line string literals!")));
				}
				else if (curr == '\n') {
					advance();
					return lexer_err(lexer_eof_err(
						m_File,
						str_beg, interval(m_Last, 1),
						ystr("A string literal was not ended before the end-of-line!"),
						ystr("Yoakke does not support multi-line string literals!")));
				}
				else if (curr == '\\') {
					// Escape sequence
					str_len++;
					advance();
					if (curr == 'n') {
						str += '\n';
						str_len++;
					}
					else if (curr == '\\') {
						str += '\\';
						str_len++;
					}
					else if (curr == '0') {
						str += '\0';
						str_len++;
					}
					else if (curr == 't') {
						str += '\t';
						str_len++;
					}
					else if (curr == '"') {
						str += '"';
						str_len++;
					}
					else {
						auto& pt = std::get<0>(m_State);
						auto ret = lexer_err(lexer_unk_tok_err(m_File,
							interval(point(pt.Column + 1, pt.Row), 1),
							ystr{ *m_Ptr }));
						advance();
						return ret;
					}
				}
				else {
					str += curr;
					str_len++;
					advance();
				}
			}
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
				return token(interval(std::get<0>(m_State), sym.length()), it->second, it->first);
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
				return token(interval(std::get<0>(m_State), num.length()),
					token_t::RealLit, num);
			}
			else {
				return token(interval(std::get<0>(m_State), num.length()),
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
				return token(interval(std::get<0>(m_State), ident.length()),
					token_t::Ident, ident);
			}
			else {
				// There is a keyword with a given type
				return token(interval(std::get<0>(m_State), ident.length()),
					it->second, ident);
			}
		}

		// Construct the error message
		auto& pt = std::get<0>(m_State);
		auto ret = lexer_err(lexer_unk_tok_err(m_File,
			interval(point(pt.Column + 1, pt.Row), 1),
			ystr{ *m_Ptr }));
		// Advance to avoid infinite loops
		advance();

		return ret;
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
	auto& pt = std::get<0>(m_State);
	for (ysize i = 0; i < delta; i++, m_Ptr++) {
		if (*m_Ptr == '\n') {
			// We have encountered a newline, go to the next line
			pt.Column = 0;
			pt.Row++;
		}
		else {
			// No new line, go to the next char
			pt.Column++;
			if (!std::isspace(*m_Ptr)) {
				// This is a visible character
				m_Last = pt;
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

yresult<token, lexer_err> lexer::peek(ysize delta) {
	auto& buff = std::get<1>(m_State);
	while (buff.size() <= delta) {
		auto res = next();
		if (res.is_ok()) {
			buff.push_back(res.get_ok());
		}
		else {
			return res.get_err();
		}
	}
	return buff[delta];
}

yresult<token, lexer_err> lexer::consume() {
	// Consuming is just erasing from the beginning of the buffer
	auto& buff = std::get<1>(m_State);
	auto tok = peek();
	if (tok.is_ok()) {
		buff.erase(buff.begin());
	}
	return tok;
}
