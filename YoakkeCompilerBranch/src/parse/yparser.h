#pragma once

#include "prec_parser.h"
#include "../ast/stmt.h"
#include "../ast/expr.h"
#include "../ast/type_desc.h"
#include "../ast/pattern.h"
#include "../ast/node_tag.h"

namespace yk {
	typedef ypair<token, type_desc*> param_pair;

	class yparser : public parser {
	private:
		prec_parser<expr> m_ExprParser;
		prec_parser<type_desc> m_TypeParser;
		prec_parser<pattern> m_PatternParser;

	public:
		yparser(token_buffer* buff);

	public:
		void prefix_expr(ystr const& op, ysize prec);
		void postfix_expr(ystr const& op, ysize prec);
		void infixl_expr(ystr const& op, ysize prec);
		void infixr_expr(ystr const& op, ysize prec);

		expr* parse_expr();
		type_desc* parse_type();
		pattern* parse_pattern();
		param_expr* parse_param();
		expr* parse_body();
		stmt* parse_stmt();
		node_tag* parse_tag();
	};
}