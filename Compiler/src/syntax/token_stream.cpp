#include "token_stream.h"

token_stream::token_stream(lexer& lex)
	: m_Lexer(lex), m_Last(token(interval(point(0, 0), 0), token_t::Epsilon)) {
}

file_hnd const& token_stream::file() const {
	return m_Lexer.file();
}

token const& token_stream::peek(ysize delta) {
	// Store the peeks in the buffer to speed up lexing
	while (delta >= m_Tokens.size()) {
		m_Tokens.push_back(m_Lexer.next());
	}
	return m_Tokens[delta];
}

token const& token_stream::last() const {
	return m_Last;
}

token token_stream::consume() {
	m_Last = peek();
	// Consuming is just erasing from the peek buffer
	m_Tokens.erase(m_Tokens.begin());
	return m_Last;
}

ysize token_stream::token_count() const {
	return m_Tokens.size();
}
