#include "parser.h"

// TODO: comment

parser::parser(lexer& lex)
	: m_Lexer(lex) {
}

AST_expr* parser::parse_expr(ysize min_prec) {
	return parse_set(m_Expr, min_prec);
}

token const& parser::peek(ysize delta) {
	while (delta <= m_Tokens.size()) {
		m_Tokens.push_back(m_Lexer.next());
	}
	return m_Tokens[delta];
}

token parser::consume() {
	token ret = peek();
	m_Tokens.erase(m_Tokens.begin());
	return ret;
}
