#include "yparser.h"
#include "expr_rules.h"
#include "pattern_rules.h"
#include "type_rules.h"
#include "../ast/ast.h"

namespace yk {
	yparser::yparser(ystr const& file)
		: parser(m_Lexer, m_Buffer), m_File(file), m_Lexer(m_File), 
		m_ExprParser(m_Lexer, m_Buffer),
		m_PatternParser(m_Lexer, m_Buffer),
		m_TypeDescParser(m_Lexer, m_Buffer) {
		// Initialize parselets
		init_expr();
		init_pat_expr();
		init_ty_expr();
	}

	void yparser::init_expr() {
		// Literals
		register_expr<expr_rules::ident>(ytoken_t::Ident);
		register_expr<expr_rules::int_lit>(ytoken_t::Integer);
		register_expr<expr_rules::real_lit>(ytoken_t::Real);

		// Enclosing
		register_expr<expr_rules::enclose>(ytoken_t::Lpar);
		register_expr<expr_rules::block>(ytoken_t::Lbrace);

		// Prefix elements
		register_expr<expr_rules::let>(ytoken_t::Let);

		// Operators
		register_expr<expr_rules::const_asgn>(ytoken_t::Dcolon, 1);
		register_expr<expr_rules::list>(ytoken_t::Comma, 2);
		register_expr<expr_rules::asgn>(ytoken_t::Asgn, 3);
		register_expr<expr_rules::lbinop>(ytoken_t::Or, 4);
		register_expr<expr_rules::lbinop>(ytoken_t::And, 5);

		register_expr<expr_rules::lbinop>(ytoken_t::Eq, 6);
		register_expr<expr_rules::lbinop>(ytoken_t::Neq, 6);

		register_expr<expr_rules::lbinop>(ytoken_t::Gr, 7);
		register_expr<expr_rules::lbinop>(ytoken_t::Le, 7);
		register_expr<expr_rules::lbinop>(ytoken_t::Goe, 7);
		register_expr<expr_rules::lbinop>(ytoken_t::Loe, 7);

		register_expr<expr_rules::lbinop>(ytoken_t::Add, 8);
		register_expr<expr_rules::lbinop>(ytoken_t::Sub, 8);

		register_expr<expr_rules::lbinop>(ytoken_t::Mul, 9);
		register_expr<expr_rules::lbinop>(ytoken_t::Div, 9);
		register_expr<expr_rules::lbinop>(ytoken_t::Mod, 9);

		register_expr<expr_rules::preury>(ytoken_t::Add, 10);
		register_expr<expr_rules::preury>(ytoken_t::Sub, 10);
		register_expr<expr_rules::preury>(ytoken_t::Not, 10);

		register_expr<expr_rules::lbinop>(ytoken_t::Dot, 11);
		register_expr<expr_rules::call>(ytoken_t::Lpar, 11);
	}

	void yparser::init_pat_expr() {
		// Literals
		register_pattern<pat_rules::ident>(ytoken_t::Ident);
		register_pattern<pat_rules::ignore>(ytoken_t::Ignore);

		// Enclose
		register_pattern<pat_rules::enclose>(ytoken_t::Lpar);

		// List
		register_pattern<pat_rules::list>(ytoken_t::Comma, 1);
	}

	void yparser::init_ty_expr() {
		// Literals
		register_type_desc<type_rules::ident>(ytoken_t::Ident);

		// Enclose
		register_type_desc<type_rules::enclose>(ytoken_t::Lpar);

		// TODO: Consider precedence of ',' and '->'
		// List
		register_type_desc<type_rules::list>(ytoken_t::Comma, 1);

		// Function
		register_type_desc<type_rules::lbinop>(ytoken_t::Arrow, 2);
	}

	yvec<stmt> yparser::parse_program() {
		yvec<stmt> ls;
		while (auto st = parse_stmt()) {
			if (auto exp = std::get_if<ysptr<expr_stmt>>(&st->Data)) {
				if (!std::get_if<ysptr<const_asgn_expr>>(&std::get<0>((*exp)->as())->Data)) {
					throw std::exception("TODO error: only const asgn can be global");
				}
			}
			ls.push_back(*st);
		}
		return ls;
	}

	yopt<expr> yparser::parse_expr(ysize prec) {
		return m_ExprParser.parse(*this, prec);
	}

	yopt<pat_expr> yparser::parse_pat_expr(ysize prec) {
		return m_PatternParser.parse(*this, prec);
	}

	yopt<ty_expr> yparser::parse_ty_expr(ysize prec) {
		return m_TypeDescParser.parse(*this, prec);
	}

	yopt<stmt> yparser::parse_stmt() {
		if (auto exp = parse_expr()) {
			return stmt(
				exp->Position,
				std::make_shared<expr_stmt>(
					std::make_shared<expr>(*exp), 
					match(ytoken_t::Semicol), false));
		}
		return {};
	}

	yopt<expr> yparser::parse_block(token const& begin) {
		yvec<stmt> body;
		while (auto st = parse_stmt()) {
			body.push_back(*st);
		}
		if (auto endbr = match(ytoken_t::Rbrace)) {
			return expr(
				begin.Position * endbr->Position,
				std::make_shared<block_expr>(body, true));
		}
		expect_error("'}'", "", *this);
	}

	file_handle const& yparser::file() const {
		return m_File;
	}

	ystr yparser::format_token(token const& t) {
		switch (ytoken_t(t.Type)) {
		case ytoken_t::Ident:
			return "identifier";

		case ytoken_t::Integer:
			return "integer literal";

		case ytoken_t::Real:
			return "real literal";

		case ytoken_t::EndOfFile:
			return "end of file";

		case ytoken_t::Epsilon:
			return "<unknown>";

		default:
			return "'" + t.Value + "'";
		}
	}
}
