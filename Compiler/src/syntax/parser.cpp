#include "ast_stmt.h"
#include "ast_ty.h"
#include "parser.h"
#include "parselet_common.h"
#include "parselet_expr.h"

parser::parser(lexer& lex)
	: m_Lexer(lex), m_Last(token(interval(point(0, 0), 0), token_t::Epsilon)) {

	// EXPRESSIONS ////////////////////////////////////////////////////////////

	// Add the literal passes
	m_Expr.add(token_t::Ident,	prefix::pass<AST_expr, AST_ident_expr>());
	m_Expr.add(token_t::IntLit, prefix::pass<AST_expr, AST_int_lit_expr>());

	// Add grouping
	m_Expr.add(token_t::LParen,
		prefix::enclose<AST_expr>(parselet_expr::get_expr, 
			token_t::RParen, "expression", "')'"));

	// Function
	m_Expr.add(token_t::Fn,
	prefix_parselet<AST_expr>(parselet_expr::get_func));

	// If
	{
		std::function<AST_if_expr*(parser&, token const&)> parse_if;
		parse_if = [&](parser& p, token const& beg) -> AST_if_expr* {
			yopt<token> ident = parselet_common::get_label(p);

			// Condition
			AST_expr* cond = parselet_expr::get_expr(p);
			if (!cond) {
				parselet_common::parse_error(p, "condition");
				return nullptr;
			}

			// Return type
			AST_ty* rett = nullptr;
			if (parselet_common::get_token<token_t::Arrow>(p)) {
				if (!(rett = p.parse_ty())) {
					parselet_common::parse_error(p, "return type");
					return nullptr;
				}
			}

			// Body
			AST_body_expr* body = parselet_expr::get_body(p);
			if (!body) {
				token beg = p.peek();
				parselet_common::parse_error(p, "body");
				return nullptr;
			}
			// Else if, else
			AST_body_expr* elbody = nullptr;
			if (auto elift = parselet_common::get_token<token_t::Elif>(p)) {
				AST_if_expr* exp = parse_if(p, *elift);
				assert(exp && "parse_if should have thrown an error!");
				elbody = new AST_body_expr(new AST_expr_stmt(exp));
			}
			else if (auto elt = parselet_common::get_token<token_t::Else>(p)) {
				elbody = parselet_expr::get_body(p);
				if (!elbody) {
					parselet_common::parse_error(p, "else block");
					return nullptr;
				}
			}

			return new AST_if_expr(beg, ident, cond, rett, body, elbody);
		};
		m_Expr.add(token_t::If, prefix_parselet<AST_expr>(parse_if));
	}

	// Expression operators

	ysize prec = 1;

	m_Expr.add(token_t::Asgn, 
		infix::rassoc<AST_expr, AST_bin_expr>(prec, 
			parselet_expr::get_expr, "expression"));

	prec++;

	m_Expr.add(token_t::Add, 
		infix::lassoc<AST_expr, AST_bin_expr>(prec, 
			parselet_expr::get_expr, "expression"));
	m_Expr.add(token_t::Sub, 
		infix::lassoc<AST_expr, AST_bin_expr>(prec, 
			parselet_expr::get_expr, "expression"));

	prec++;

	m_Expr.add(token_t::Mul, 
		infix::lassoc<AST_expr, AST_bin_expr>(prec, 
			parselet_expr::get_expr, "expression"));
	m_Expr.add(token_t::Div, 
		infix::lassoc<AST_expr, AST_bin_expr>(prec, 
			parselet_expr::get_expr, "expression"));
	m_Expr.add(token_t::Mod, 
		infix::lassoc<AST_expr, AST_bin_expr>(prec, 
			parselet_expr::get_expr, "expression"));

	prec++;

	m_Expr.add(token_t::Eq,
		infix::lassoc<AST_expr, AST_bin_expr>(prec, 
			parselet_expr::get_expr, "expression"));
	m_Expr.add(token_t::Neq,
		infix::lassoc<AST_expr, AST_bin_expr>(prec, 
			parselet_expr::get_expr, "expression"));

	prec++;

	m_Expr.add(token_t::LParen,
		infix_parselet<AST_expr>(
		prec,
		[](parser& p, AST_expr* left, token const& lpar) -> AST_expr* {
			// TODO: Change when making comma separated list expressions
			yvec<AST_expr*> params;
			if (AST_expr* par = p.parse_expr()) {
				params.push_back(par);
				while (parselet_common::get_token<token_t::Comma>(p)) {
					if (AST_expr* par2 = p.parse_expr()) {
						params.push_back(par2);
					}
					else {
						parselet_common::parse_error(p, "parameter");
						return nullptr;
					}
				}
			}
			if (auto rpar = parselet_common::get_token<token_t::RParen>(p)) {
				return new AST_call_expr(left, params, *rpar);
			}
			else {
				parselet_common::parse_error(p, "')'");
				return nullptr;
			}
		},
		[](parser& p) -> bool {
			return p.last().Type != token_t::RBrace;
		})
	);

	// TYPES //////////////////////////////////////////////////////////////////

	// Create a combinator that parses a type with a given precedence
	auto get_ty = [](parser& p, ysize prec) {
		return p.parse_ty(prec);
	};

	// Add the literal passes
	m_Ty.add(token_t::Ident, prefix::pass<AST_ty, AST_ident_ty>());

	// Add grouping
	m_Ty.add(token_t::LParen,
		prefix::enclose<AST_ty>(get_ty, token_t::RParen, "type", "')'"));
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
