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

		class pre_uryop : public expr_pre_parselet {
		public:
			ysize precedence;

		public:
			pre_uryop(ysize prec)
				: precedence(prec) {
			}

		public:
			expr* parse(token const& begin, expr_parser* parser) override {
				auto right = parser->parse(precedence);
				if (right) {
					return new preury_expr(right, begin);
				}
				throw std::exception("RHS expected!");
			}
		};

		class post_uryop : public expr_in_parselet {
		public:
			post_uryop(ysize prec)
				: expr_in_parselet(prec) {
			}

		public:
			expr* parse(expr* left, token const& begin, expr_parser* parser) override {
				return new postury_expr(left, begin);
			}
		};

		class enclosed : public expr_pre_parselet {
		public:
			ystr left;
			ystr right;

		public:
			enclosed(ystr const& l, ystr const& r)
				: left(l), right(r) {
			}

		public:
			expr* parse(token const& begin, expr_parser* parser) override {
				auto expr = parser->parse();
				auto end = parser->match_id(right);
				if (end.some()) {
					return new enclose_expr(expr, begin, end.get());
				}
				else {
					throw std::exception("Enclosed RHS expected!");
				}
			}
		};
	}

	yexpr_parser::yexpr_parser(token_buffer* buff)
		: prec_parser<expr>(buff) {
		// Atomic
		register_rule("Identifier", new expr_rules::ident());

		// Operators
		infixr("=", 1);
		infixl("+", 2);
		infixl("-", 2);
		infixl("*", 3);
		infixl("/", 3);
	}

	void yexpr_parser::prefix(ystr const& op, ysize prec) {
		register_rule(op, new expr_rules::pre_uryop(prec));
	}

	void yexpr_parser::postfix(ystr const& op, ysize prec) {
		register_rule(op, new expr_rules::post_uryop(prec));
	}

	void yexpr_parser::infixl(ystr const& op, ysize prec) {
		register_rule(op, new expr_rules::binop(prec, false));
	}

	void yexpr_parser::infixr(ystr const& op, ysize prec) {
		register_rule(op, new expr_rules::binop(prec, true));
	}
}