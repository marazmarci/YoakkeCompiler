#include "ast_stmt.h"
#include "ast_ty.h"
#include "parser.h"

parser::parser(lexer& lex)
	: m_Lexer(lex), m_Last(token(interval(point(0, 0), 0), token_t::Epsilon)) {

	// EXPRESSIONS ////////////////////////////////////////////////////////////

	// Create a combinator for parsing an optional single token
	auto get_tok = [](parser& p, token_t tt) -> yopt<token> {
		if (p.peek().Type == tt) {
			return p.consume();
		}
		return {};
	};

	// Create a combinator for labels
	auto get_label = [&get_tok](parser& p) -> yopt<token> {
		if (auto lbracket = get_tok(p, token_t::LBracket)) {
			if (auto id = get_tok(p, token_t::Ident)) {
				if (auto rbracket = get_tok(p, token_t::RBracket)) {
					return id;
				}
				else {
					token ahead = p.peek();
					throw parser_expect_exception(p.file(), ahead.Pos,
						"']'", ahead.fmt(), lbracket->Pos);
					return {};
				}
			}
			else {
				token ahead = p.peek();
				throw parser_expect_exception(p.file(), ahead.Pos,
					"identifier", ahead.fmt());
				return {};
			}
		}
		else {
			return {};
		}
	};

	// Create a combinator that parses an expression with a given precedence
	auto get_expr = [](parser& p, ysize prec) {
		return p.parse_expr(prec);
	};

	// Create a combinator that parses a body for braced constructs
	auto get_body = [](parser& p) -> AST_body_expr* {
		// TODO: capture list
		token lbrace = p.peek();
		if (lbrace.Type != token_t::LBrace) {
			return nullptr;
		}
		p.consume();
		// { parsed
		yvec<AST_stmt*> statements;
		while (AST_stmt* stmt = p.parse_stmt()) {
			statements.push_back(stmt);
		}
		// Now we check if there is a last expression
		AST_expr* ret = p.parse_expr();
		// Expect a }
		token rbrace = p.peek();
		if (rbrace.Type != token_t::RBrace) {
			throw parser_expect_exception(p.file(), rbrace.Pos,
				"'}'", rbrace.fmt(), lbrace.Pos);
		}
		return new AST_body_expr(lbrace, rbrace, statements, ret);
	};

	// Add the literal passes
	m_Expr.add(token_t::Ident,	prefix::pass<AST_expr, AST_ident_expr>());
	m_Expr.add(token_t::IntLit, prefix::pass<AST_expr, AST_int_lit_expr>());

	// Add grouping
	m_Expr.add(token_t::LParen,
		prefix::enclose<AST_expr>(get_expr, token_t::RParen, "expression", "')'"));

	// Function
	m_Expr.add(token_t::Fn,
	prefix_parselet<AST_expr>(
	[&get_body, &get_label, &get_tok](parser& p, token const& beg) -> AST_expr* {
		yopt<token> ident = get_label(p);
		// Params
		auto lpar = get_tok(p, token_t::LParen);

		// Parse params
		auto get_param = [&get_tok](parser& p) -> yopt<param_t> {
			auto ident = get_tok(p, token_t::Ident);
			if (!get_tok(p, token_t::Colon)) {
				if (ident) {
					auto ahead = p.peek();
					throw parser_expect_exception(p.file(), ahead.Pos,
						"':'", ahead.fmt());
					return {};
				}
				else {
					return {};
				}
			}
			AST_ty* ty = p.parse_ty();
			if (!ty) {
				auto ahead = p.peek();
				throw parser_expect_exception(p.file(), ahead.Pos,
					"type", ahead.fmt());
				return {};
			}
			return param_t({ ident, ty });
		};

		yvec<param_t> params;
		{
			if (auto par = get_param(p)) {
				params.push_back(*par);
				while (get_tok(p, token_t::Comma)) {
					if (auto par2 = get_param(p)) {
						params.push_back(*par2);
					}
					else {
						auto ahead = p.peek();
						throw parser_expect_exception(p.file(), ahead.Pos,
							"parameter", ahead.fmt());
						return {};
					}
				}
			}
		}

		if (lpar) {
			token rpar = p.peek();
			if (rpar.Type != token_t::RParen) {
				throw parser_expect_exception(p.file(), rpar.Pos,
					"')'", rpar.fmt(), lpar->Pos);
				return nullptr;
			}
			p.consume();
		}

		// Return type
		AST_ty* rett = nullptr;
		if (get_tok(p, token_t::Arrow)) {
			if (!(rett = p.parse_ty())) {
				auto ahead = p.peek();
				throw parser_expect_exception(p.file(), ahead.Pos,
					"return type", ahead.fmt());
				return nullptr;
			}
		}

		// Body
		AST_body_expr* body = get_body(p);
		if (!body) {
			token beg = p.peek();
			throw parser_expect_exception(p.file(), beg.Pos,
				"body", beg.fmt());
			return nullptr;
		}
		return new AST_func_expr(beg, params, rett, body, ident);
	}));

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

	prec++;

	m_Expr.add(token_t::LParen,
		infix_parselet<AST_expr>(
		prec,
		[&get_tok](parser& p, AST_expr* left, token const& lpar) -> AST_expr* {
			// TODO: Change when making comma separated list expressions
			yvec<AST_expr*> params;
			if (AST_expr* par = p.parse_expr()) {
				params.push_back(par);
				while (get_tok(p, token_t::Comma)) {
					if (AST_expr* par2 = p.parse_expr()) {
						params.push_back(par2);
					}
					else {
						auto ahead = p.peek();
						throw parser_expect_exception(p.file(), ahead.Pos,
							"parameter", ahead.fmt());
						return nullptr;
					}
				}
			}
			if (auto rpar = get_tok(p, token_t::RParen)) {
				return new AST_call_expr(left, params, *rpar);
			}
			else {
				auto ahead = p.peek();
				throw parser_expect_exception(p.file(), ahead.Pos,
					"')'", ahead.fmt());
				return nullptr;
			}
		},
		[](parser& p) -> bool {
			return p.last().Type != token_t::RBrace;
		})
	);

	// TYPES //////////////////////////////////////////////////////////////////

	// Add the literal passes
	m_Ty.add(token_t::Ident, prefix::pass<AST_ty, AST_ident_ty>());
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
