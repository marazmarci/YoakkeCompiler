#include "yparser.h"
#include "../ast/expr.h"

namespace yk {
	namespace expr_rules {
		using expr_prefix_parselet = prefix_parselet<expr, yparser>;

		class ident : public prefix_parselet<expr, yparser> {
		public:
			virtual yshared_ptr<expr> parse(token const& begin, yparser& parser) override {
				return std::make_shared<ident_expr>(begin);
			}
		};
	}

	yparser::yparser(ystr const& file)
		: parser(m_Lexer, m_Buffer), m_Lexer(file), m_ExprParser(m_Lexer, m_Buffer) {
		m_ExprParser.register_rule(ytoken_t::Ident, std::make_shared<expr_rules::ident>());
	}

	yshared_ptr<expr> yparser::parse_expr() {
		return m_ExprParser.parse(*this);
	}
}
