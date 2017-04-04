#pragma once

#include "rules.h"
#include "yparser.h"
#include "../ast/expr.h"

namespace yk {
	namespace expr_rules {
		using expr_pre_parselet = prefix_parselet<expr, yparser>;
		using expr_in_parselet = infix_parselet<expr, yparser>;

		template <typename Return_T>
		class pass : public expr_pre_parselet {
		public:
			yshared_ptr<expr> parse(token const& begin, yparser& parser) override {
				return std::make_shared<Return_T>(begin);
			}
		};

		using ident = pass<ident_expr>;
		using int_lit = pass<int_lit_expr>;
		using real_lit = pass<real_lit_expr>;

		class enclose : public expr_pre_parselet {
		public:
			yshared_ptr<expr> parse(token const& begin, yparser& parser) override {
				if (auto sub = parser.parse_expr()) {
					if (parser.match(ytoken_t::Rpar)) {
						return sub;
					}
					else {
						token const& tok = parser.peek();
						throw_expect("')'");
					}
				}
				else {
					token const& tok = parser.peek();
					throw_expect("expression");
				}
			}
		};

		class block : public expr_pre_parselet {
		public:
			yshared_ptr<expr> parse(token const& begin, yparser& parser) override {
				yvec<yshared_ptr<stmt>> body;
				while (auto st = parser.parse_stmt()) {
					body.push_back(st);
				}
				if (auto endbr = parser.match(ytoken_t::Rbrace)) {
					return std::make_shared<block_expr>(body, begin, endbr.value());
				}
				else {
					throw_expect("'}'");
				}
			}
		};

		class preury : public expr_pre_parselet {
		private:
			ysize m_Precedence;

		public:
			preury(ysize prec)
				: m_Precedence(prec) {
			}

		public:
			yshared_ptr<expr> parse(token const& begin, yparser& parser) override {
				if (auto right = parser.parse_expr(m_Precedence)) {
					return std::make_shared<preury_expr>(begin, right);
				}
				else {
					throw_expect("right-hand side operand");
				}
			}
		};

		class let : public expr_pre_parselet {
		public:
			yshared_ptr<expr> parse(token const& begin, yparser& parser) override {
				if (auto pat = parser.parse_pattern()) {
					yshared_ptr<type_desc> type = nullptr;
					if (parser.match(ytoken_t::Colon)) {
						if (type = parser.parse_type_desc()) {
						}
						else {
							throw_expect("type");
						}
					}
					yshared_ptr<type_desc> val = nullptr;
					if (parser.match(ytoken_t::Asgn)) {
						if (val = parser.parse_expr()) {
						}
						else {
							throw_expect("expression");
						}
					}
					return std::make_shared<let_expr>(pat, type, val, begin);
				}
				else {
					throw_expect("pattern");
				}
			}
		};

		class postury : public expr_in_parselet {
		public:
			postury(ysize prec)
				: expr_in_parselet(prec) {
			}

		public:
			yshared_ptr<expr> parse(
				yshared_ptr<expr> left, token const& begin, yparser& parser) override {
				return std::make_shared<postury_expr>(begin, left);
			}
		};

		class list : public expr_in_parselet {
		public:
			list(ysize prec)
				: expr_in_parselet(prec) {
			}

		public:
			yshared_ptr<expr> parse(
				yshared_ptr<expr> left, token const& begin, yparser& parser) override {
				auto ls = std::make_shared<list_expr>(left);
				do {
					if (auto rhs = parser.parse_expr(precedence() - 1)) {
						ls->add(rhs);
					}
					else {
						throw_expect("expression");
					}
				} while (parser.match(ytoken_t::Comma));
				return ls;
			}
		};

		class call : public expr_in_parselet {
		public:
			call(ysize prec)
				: expr_in_parselet(prec) {
			}

		public:
			yshared_ptr<expr> parse(
				yshared_ptr<expr> left, token const& begin, yparser& parser) override {
				auto args = parser.parse_expr();
				if (auto tok = parser.match(ytoken_t::Rpar)) {
					return std::make_shared<call_expr>(left, args, tok.value());
				}
				else {
					throw_expect("')'");
				}
			}

			bool matches(yshared_ptr<expr> left, yparser& parser) override {
				return parser.last().Type != (std::size_t)ytoken_t::Rbrace;
			}
		};

		template <bool Right, typename Return_T>
		class bin : public expr_in_parselet {
		public:
			bin(ysize prec)
				: expr_in_parselet(prec) {
			}

		public:
			yshared_ptr<expr> parse(
				yshared_ptr<expr> left, token const& begin, yparser& parser) override {
				if (auto rhs = parser.parse_expr(precedence() - (Right ? 1 : 0))) {
					return std::make_shared<Return_T>(begin, left, rhs);
				}
				else {
					throw_expect("expression");
				}
			}
		};

		class const_asgn : public bin<true, const_asgn_expr> {
		public:
			const_asgn(ysize prec)
				: bin(prec) {
			}

		public:
			bool matches(yshared_ptr<expr> left, yparser& parser) override {
				return std::dynamic_pointer_cast<ident_expr>(left) != nullptr;
			}
		};

		using lbinop = bin<false, binop_expr>;
		using rbinop = bin<true, binop_expr>;
		using asgn = bin<true, asgn_expr>;
	}
}
