#include "ast_stmt.h"
#include "ast_ty.h"
#include "parser.h"

parser::parser(lexer& lex)
	: m_Lexer(lex), m_Last(token(interval(point(0, 0), 0), token_t::Epsilon)) {

	// EXPRESSIONS ////////////////////////////////////////////////////////////

	// Add the literal passes
	m_Expr.add(token_t::Ident,	
	prefix_parselet<AST_expr>(parselet::from_term<AST_ident_expr>));
	m_Expr.add(token_t::IntLit,
	prefix_parselet<AST_expr>(parselet::from_term<AST_int_lit_expr>));

	// Add grouping
	m_Expr.add(token_t::LParen,
	prefix_parselet<AST_expr>(
		parselet::make_enclose<token_t::RParen>(
			parselet::get_expr_p<0>,
			"expression", "')'")));

	// Function
	m_Expr.add(token_t::Fn,
	prefix_parselet<AST_expr>(parselet::get_func));

	// If statement
	m_Expr.add(token_t::If,
		prefix_parselet<AST_expr>(parselet::get_if));

	// Expression operators

	ysize prec = 1;

	m_Expr.add(token_t::Asgn, 
	infix_parselet<AST_expr>(prec,
		parselet::make_rassoc<AST_bin_expr, AST_expr>
		(parselet::get_expr, "expression")));

	prec++;

	m_Expr.add(token_t::Add, 
		infix_parselet<AST_expr>(prec,
			parselet::make_lassoc<AST_bin_expr, AST_expr>
			(parselet::get_expr, "expression")));
	m_Expr.add(token_t::Sub, 
		infix_parselet<AST_expr>(prec,
			parselet::make_lassoc<AST_bin_expr, AST_expr>
			(parselet::get_expr, "expression")));

	prec++;

	m_Expr.add(token_t::Mul, 
		infix_parselet<AST_expr>(prec,
			parselet::make_lassoc<AST_bin_expr, AST_expr>
			(parselet::get_expr, "expression")));
	m_Expr.add(token_t::Div, 
		infix_parselet<AST_expr>(prec,
			parselet::make_lassoc<AST_bin_expr, AST_expr>
			(parselet::get_expr, "expression")));
	m_Expr.add(token_t::Mod, 
		infix_parselet<AST_expr>(prec,
			parselet::make_lassoc<AST_bin_expr, AST_expr>
			(parselet::get_expr, "expression")));

	prec++;

	m_Expr.add(token_t::Eq,
		infix_parselet<AST_expr>(prec,
			parselet::make_lassoc<AST_bin_expr, AST_expr>
			(parselet::get_expr, "expression")));
	m_Expr.add(token_t::Neq,
		infix_parselet<AST_expr>(prec,
			parselet::make_lassoc<AST_bin_expr, AST_expr>
			(parselet::get_expr, "expression")));

	prec++;

	// TYPES //////////////////////////////////////////////////////////////////

	// Add the literal passes
	m_Ty.add(token_t::Ident,
		prefix_parselet<AST_ty>(parselet::from_term<AST_ident_ty>));

	// Add grouping
	m_Ty.add(token_t::LParen,
	prefix_parselet<AST_ty>(
		parselet::make_enclose<token_t::RParen>(
			parselet::get_ty_p<0>,
			"type", "')'")));
}

parser_state parser::get_state() const {
	return parser_state(m_Lexer.get_state(), m_Tokens, m_Last);
}

void parser::set_state(parser_state const& st) {
	m_Lexer.set_state(st.LexState);
	m_Tokens = st.Tokens;
	m_Last = st.Last;
}

file_hnd const& parser::file() const {
	return m_Lexer.file();
}

AST_expr* parser::parse_expr(ysize min_prec) {
	return parse_set(m_Expr, min_prec);
}

AST_ty* parser::parse_ty(ysize min_prec) {
	return parse_set(m_Ty, min_prec);
}

AST_stmt* parser::parse_stmt() {
	// Try expression statement
	parser_state state = get_state();
	AST_expr* exp = parse_expr();
	if (!exp) {
		set_state(state);
		return nullptr;
	}
	if (!exp->is_braced()) {
		// If not braced, we need a semicolon
		token semicol = peek();
		if (semicol.Type != token_t::Semicol) {
			set_state(state);
			return nullptr;
		}
		consume();
		return new AST_expr_stmt(exp, semicol);
	}
	else {
		return new AST_expr_stmt(exp);
	}
}

token const& parser::peek(ysize delta) {
	// Store the peeks in the buffer to speed up lexing
	while (delta >= m_Tokens.size()) {
		m_Tokens.push_back(m_Lexer.next());
	}
	return m_Tokens[delta];
}

token const& parser::last() const {
	return m_Last;
}

token parser::consume() {
	m_Last = peek();
	// Consuming is just erasing from the peek buffer
	m_Tokens.erase(m_Tokens.begin());
	return m_Last;
}
