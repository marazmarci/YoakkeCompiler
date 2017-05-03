#pragma once

#include "parser.h"
#include "prec_parser.h"
#include "../lexing/ylexer.h"
#include "../utility/file_handle.h"

namespace yk {
	class expr;
	class pat_expr;
	class ty_expr;
	class stmt;
	class block_expr;

	class yparser : public parser {
	private:
		file_handle m_File;
		yvec<token> m_Buffer;
		ylexer m_Lexer;

		prec_parser<expr, yparser>		m_ExprParser;
		prec_parser<pat_expr, yparser>	m_PatternParser;
		prec_parser<ty_expr, yparser> m_TypeDescParser;

	public:
		yparser(ystr const& file);

	public:
		yvec<ysptr<stmt>> parse_program();
		
		ysptr<expr> parse_expr(ysize prec = 0);
		ysptr<pat_expr> parse_pattern(ysize prec = 0);
		ysptr<ty_expr> parse_type_desc(ysize prec = 0);
		ysptr<stmt> parse_stmt();
		ysptr<block_expr> parse_block(token const& begin);

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
		void init_pattern();
		void init_type_desc();

	public:
		static ystr format_token(token const& t);
	};
}
