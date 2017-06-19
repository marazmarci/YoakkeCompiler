#include "parser.h"

parser::parser(lexer& lex)
	: m_Lexer(lex) {

	// Create a combinator that parses an expression with a given precedence
	auto get_expr = [](parser& p, ysize prec) {
		return p.parse_expr(prec);
	};

	// Add the literal passes
	m_Expr.add(token_t::Ident,	prefix::pass<AST_ident_expr>());
	m_Expr.add(token_t::IntLit, prefix::pass<AST_int_lit_expr>());

	// Expression operators

	ysize prec = 1;

	m_Expr.add(token_t::Asgn, 
		infix::rassoc<AST_expr, AST_bin_expr>(prec, get_expr, "expression"));

	prec++;

	m_Expr.add(token_t::Add, 
		infix::lassoc<AST_expr, AST_bin_expr>(prec, get_expr, "expression"));
	m_Expr.add(token_t::Sub, 
		infix::lassoc<AST_expr, AST_bin_expr>(prec, get_expr, "expression"));

	prec++;

	m_Expr.add(token_t::Mul, 
		infix::lassoc<AST_expr, AST_bin_expr>(prec, get_expr, "expression"));
	m_Expr.add(token_t::Div, 
		infix::lassoc<AST_expr, AST_bin_expr>(prec, get_expr, "expression"));
	m_Expr.add(token_t::Mod, 
		infix::lassoc<AST_expr, AST_bin_expr>(prec, get_expr, "expression"));
}

file_hnd const& parser::file() const {
	return m_Lexer.file();
}

AST_expr* parser::parse_expr(ysize min_prec) {
	return parse_set(m_Expr, min_prec);
}

token const& parser::peek(ysize delta) {
	// Store the peeks in the buffer to speed up lexing
	while (delta >= m_Tokens.size()) {
		m_Tokens.push_back(m_Lexer.next());
	}
	return m_Tokens[delta];
}

token parser::consume() {
	token ret = peek();
	// Consuming is just erasing from the peek buffer
	m_Tokens.erase(m_Tokens.begin());
	return ret;
}
