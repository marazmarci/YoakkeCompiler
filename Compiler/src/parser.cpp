#include "parser.h"

// TODO: comment

parser::parser(lexer& lex)
	: m_Lexer(lex) {

	m_Expr.add(token_t::Ident,	prefix::pass<AST_ident_expr>());
	m_Expr.add(token_t::IntLit, prefix::pass<AST_int_lit_expr>());
}

AST_expr* parser::parse_expr(ysize min_prec) {
	return parse_set(m_Expr, min_prec);
}

token const& parser::peek(ysize delta) {
	while (delta >= m_Tokens.size()) {
		m_Tokens.push_back(m_Lexer.next());
	}
	return m_Tokens[delta];
}

token parser::consume() {
	token ret = peek();
	m_Tokens.erase(m_Tokens.begin());
	return ret;
}
