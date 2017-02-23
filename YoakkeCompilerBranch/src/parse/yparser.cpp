#include "yparser.h"
#include "../ast/expr.h"
#include "../utility/map_ext.h"

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
			enclosed() { }

		public:
			expr* parse(token const& begin, expr_parser* parser) override {
				expr* inner = nullptr;
				if (parser->peek().identifier() != ")") {
					inner = parser->parse();
					if (!inner) {
						throw std::exception("Expression expected between parenthesis!");
					}
				}
				auto end = parser->match_id(")");
				if (end.some()) {
					return new enclose_expr(inner, begin, end.get());
				}
				else {
					throw std::exception("')' expected!");
				}
			}
		};

		class func_call : public expr_in_parselet {
		public:
			func_call(ysize prec)
				: expr_in_parselet(prec) {
			}

		public:
			bool matches(expr* exp) override {
				return dynamic_cast<block_expr*>(exp) == nullptr;
			}

			expr* parse(expr* left, token const& begin, expr_parser* parser) override {
				yvec<expr*> operands;
				operands.push_back(left);
				auto last = parser->peek();
				if (last.identifier() != ")") {
					auto params = parser->parse();
					if (!params) {
						throw "Parameters expected!";
					}
					operands.push_back(params);
					last = parser->peek();
					if (last.identifier() != ")") {
						throw "')' expected!";
					}
				}
				parser->consume();

				return new mixfix_expr("call", operands,
					position::interval(left->Position, position::get(last)));
			}
		};

		class const_asgn : public binop {
		public:
			const_asgn(ysize prec)
				: binop(prec, true) {
			}

		public:
			bool matches(expr* left) override {
				return dynamic_cast<ident_expr*>(left) != nullptr;
			}
		};
	}

	yparser::yparser(token_buffer* buff)
		: prec_parser<expr>(buff), prec_parser<type_desc>(buff) {
		// Atomic
		prec_parser<expr>::register_rule("Identifier", new expr_rules::ident());
		prec_parser<expr>::register_rule("(", new expr_rules::enclosed());

		// Operators
		infixl(",", 1);
		
		infixr("=", 2);
		
		infixl("||", 3);

		infixl("&&", 4);

		infixl("==", 5);
		infixl("<>", 5);

		infixl("<", 6);
		infixl(">", 6);
		infixl("<=", 6);
		infixl(">=", 6);

		infixl("+", 7);
		infixl("-", 7);

		infixl("*", 8);
		infixl("/", 8);
		infixl("%", 8);

		prefix("+", 9);
		prefix("-", 9);
		prefix("!", 9);

		infixl(".", 10);
		prec_parser<expr>::register_rule("(", new expr_rules::func_call(10));
	}

	expr* yparser::parse_expr() {
		return prec_parser<expr>::parse();
	}

	void yparser::prefix(ystr const& op, ysize prec) {
		prec_parser<expr>::register_rule(op, new expr_rules::pre_uryop(prec));
	}

	void yparser::postfix(ystr const& op, ysize prec) {
		prec_parser<expr>::register_rule(op, new expr_rules::post_uryop(prec));
	}

	void yparser::infixl(ystr const& op, ysize prec) {
		prec_parser<expr>::register_rule(op, new expr_rules::binop(prec, false));
	}

	void yparser::infixr(ystr const& op, ysize prec) {
		prec_parser<expr>::register_rule(op, new expr_rules::binop(prec, true));
	}
}