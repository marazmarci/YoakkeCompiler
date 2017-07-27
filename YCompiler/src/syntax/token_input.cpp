#include <cassert>
#include "lexer.h"
#include "token.h"
#include "token_input.h"

token_input::token_input(lexer& lex)
	: m_Lexer(lex), m_Index(0) {
}

token_input const& token_input::operator=(token_input const& other) {
	assert(&m_Lexer == &other.m_Lexer && "The lexers must match!");
	m_Index = other.m_Index;
	return *this;
}

yresult<token, lexer_err> token_input::head() {
	return m_Lexer.peek(m_Index);
}

token_input token_input::tail() {
	return token_input(m_Lexer, m_Index + 1);
}

lexer const& token_input::get_lexer() const {
	return m_Lexer;
}

ysize token_input::get_index() const {
	return m_Index;
}

token_input::token_input(lexer& lex, ysize idx) 
	: m_Lexer(lex), m_Index(idx) {
}
