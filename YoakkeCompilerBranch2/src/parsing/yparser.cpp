#include "yparser.h"
#include "../ast/expr.h"

namespace yk {
	namespace expr_rules {
		using expr_prefix_parselet = prefix_parselet<expr, yparser>;
		using expr_infix_parselet = infix_parselet<expr, yparser>;

		class ident : public expr_prefix_parselet {
		public:
			virtual yshared_ptr<expr> parse(token const& begin, yparser& parser) override {
				return std::make_shared<ident_expr>(begin);
			}
		};

		class binop : public expr_infix_parselet {
		private:
			bool m_Right;

		public:
			binop(ysize prec, bool r)
				: expr_infix_parselet(prec), m_Right(r) {
			}

		public:
			virtual yshared_ptr<expr> parse(
				yshared_ptr<expr> left, token const& begin, yparser& parser) override {
				auto rhs = parser.parse_expr(precedence() - (m_Right ? 1 : 0));
				if (rhs) {
					return std::make_shared<bin_expr>(begin, left, rhs);
				}
				else {
					throw std::exception("RHS expected for operator!");
				}
			}
		};
	}

	yparser::yparser(ystr const& file)
		: parser(m_Lexer, m_Buffer), m_Lexer(file), m_ExprParser(m_Lexer, m_Buffer) {
		m_ExprParser.register_rule(ytoken_t::Ident, std::make_shared<expr_rules::ident>());
	}

	yshared_ptr<expr> yparser::parse_expr(ysize prec) {
		return m_ExprParser.parse(*this, prec);
	}
}
