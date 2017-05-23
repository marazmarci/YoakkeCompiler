#pragma once

#include "parser.h"
#include "prec_parser.h"
#include "../lexing/ylexer.h"
#include "../utility/file_handle.h"

namespace yk {
	struct expr;
	struct pat_expr;
	struct ty_expr;
	struct stmt;

	class yparser : public parser {
	private:
		file_handle m_File;
		yvec<token> m_Buffer;
		ylexer m_Lexer;

		prec_parser<expr, yparser>		m_ExprParser;
		prec_parser<pat_expr, yparser>	m_PatternParser;
		prec_parser<ty_expr, yparser>	m_TypeDescParser;

	public:
		yparser(ystr const& file);

	public:
		yvec<stmt> parse_program();
		
		yopt<expr> parse_expr(ysize prec = 0);
		yopt<pat_expr> parse_pat_expr(ysize prec = 0);
		yopt<ty_expr> parse_ty_expr(ysize prec = 0);
		yopt<stmt> parse_stmt();
		yopt<expr> parse_block(token const& begin);

		file_handle const& file() const;

	private:
		template <typename T, typename... Args>
		void register_expr(ytoken_t tt, Args... args) {
			m_ExprParser.register_rule(tt, std::make_shared<T>(args...));
		}

		template <typename T, typename... Args>
		void register_pattern(ytoken_t tt, Args... args) {
			m_PatternParser.register_rule(tt, std::make_shared<T>(args...));
		}

		template <typename T, typename... Args>
		void register_type_desc(ytoken_t tt, Args... args) {
			m_TypeDescParser.register_rule(tt, std::make_shared<T>(args...));
		}

		void init_expr();
		void init_pat_expr();
		void init_ty_expr();
	};
}
