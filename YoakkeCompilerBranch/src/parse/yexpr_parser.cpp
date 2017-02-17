#include "yexpr_parser.h"
#include "../ast/expr.h"

namespace yk {
	namespace expr_rules {
		typedef prec_parser<expr> expr_parser;
		typedef prefix_parselet<expr, expr_parser> expr_pre_parselet;
		typedef infix_parselet<expr, expr_parser> expr_in_parselet;

		class ident : public expr_pre_parselet {
		public:
			expr* parse(token const& begin, expr_parser* parser) override {
				return new ident_expr(begin);
			}
		};

		class binop : public expr_in_parselet {
		public:
			const bool right;

		public:
			binop(ysize prec, bool r)
				: expr_in_parselet(prec), right(r) {
			}

		public:
			expr* parse(expr* left, token const& begin, expr_parser* parser) override {
				auto rhs = parser->parse(precedence() - (right ? 1 : 0));
				if (rhs) {
					return new bin_expr(left, rhs, begin);
				}
				else {
					throw std::exception("RHS expected for operator!");
				}
			}
		};
	}

	yexpr_parser::yexpr_parser(token_buffer* buff)
		: prec_parser<expr>(buff) {
		register_rule("Identifier", new expr_rules::ident());
	}

	void yexpr_parser::infixl(ystr const& op, ysize prec) {
		register_rule(op, new expr_rules::binop(prec, false));
	}

	void yexpr_parser::infixr(ystr const& op, ysize prec) {
		register_rule(op, new expr_rules::binop(prec, true));
	}
}