#include "lexer.h"
#include "token.h"
#include "token_input.h"

token_input::token_input(lexer& lex)
	: m_Lexer(lex), m_Index(0) {
}

token& token_input::head() {
	return m_Lexer.peek();
}

token_input token_input::tail() {
	return token_input(m_Lexer, m_Index + 1);
}

token_input::token_input(lexer& lex, ysize idx) 
	: m_Lexer(lex), m_Index(idx) {
}
