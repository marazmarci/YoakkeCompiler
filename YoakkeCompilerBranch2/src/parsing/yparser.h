#pragma once

#include "parser.h"
#include "prec_parser.h"
#include "../lexing/ylexer.h"

namespace yk {
	class expr;
	class pattern;
	class type_desc;
	class stmt;
	class block_expr;

	class yparser : public parser {
	private:
		ystr m_File;
		yvec<token> m_Buffer;
		ylexer m_Lexer;

		prec_parser<expr, yparser>		m_ExprParser;
		prec_parser<pattern, yparser>	m_PatternParser;
		prec_parser<type_desc, yparser> m_TypeDescParser;

	public:
		yparser(ystr const& file);

	public:
		yshared_ptr<expr> parse_expr(ysize prec = 0);
		yshared_ptr<pattern> parse_pattern(ysize prec = 0);
		yshared_ptr<type_desc> parse_type_desc(ysize prec = 0);
		yshared_ptr<stmt> parse_stmt();
		yshared_ptr<block_expr> parse_block(token const& begin);

		ystr const& file() const;

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
		void init_pattern();
		void init_type_desc();

	public:
		static ystr format_token(token const& t);
	};
}
