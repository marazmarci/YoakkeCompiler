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

		class int_lit : public expr_prefix_parselet {
		public:
			virtual yshared_ptr<expr> parse(token const& begin, yparser& parser) override {
				return std::make_shared<int_lit_expr>(begin);
			}
		};

		class real_lit : public expr_prefix_parselet {
		public:
			virtual yshared_ptr<expr> parse(token const& begin, yparser& parser) override {
				return std::make_shared<real_lit_expr>(begin);
			}
		};

		class enclose : public expr_prefix_parselet {
		public:
			virtual yshared_ptr<expr> parse(token const& begin, yparser& parser) override {
				if (auto sub = parser.parse_expr()) {
					if (parser.match(ytoken_t::Rpar)) {
						return sub;
					}
					else {
						throw std::exception("')' expected!");
					}
				}
				else {
					throw std::exception("WAT");
				}
			}
		};

		template <ysize PREC, bool RIGHT, typename RETT>
		class binop : public expr_infix_parselet {
		public:
			virtual yshared_ptr<expr> parse(
				yshared_ptr<expr> left, token const& begin, yparser& parser) override {
				if (auto rhs = parser.parse_expr(precedence() - (RIGHT ? 1 : 0))) {
					return std::make_shared<RETT>(begin, left, rhs);
				}
				else {
					throw std::exception("RHS expected for operator!");
				}
			}
		};
	}

	yparser::yparser(ystr const& file)
		: parser(m_Lexer, m_Buffer), m_Lexer(file), m_ExprParser(m_Lexer, m_Buffer) {
		// Literals
		register_expr<expr_rules::ident>	(ytoken_t::Ident);
		register_expr<expr_rules::int_lit>	(ytoken_t::Integer);
		register_expr<expr_rules::real_lit>	(ytoken_t::Real);

		// Enclosing
		register_expr<expr_rules::enclose>	(ytoken_t::Lpar);
	}

	yshared_ptr<expr> yparser::parse_expr(ysize prec) {
		return m_ExprParser.parse(*this, prec);
	}
}
