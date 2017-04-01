#pragma once

#include "rules.h"
#include "yparser.h"
#include "../ast/pattern.h"

namespace yk {
	namespace pat_rules {
		using pat_pre_parselet = prefix_parselet<pattern, yparser>;
		using pat_in_parselet = infix_parselet<pattern, yparser>;

		template <typename Return_T>
		class pass : public pat_pre_parselet {
		public:
			yshared_ptr<pattern> parse(token const& begin, yparser& parser) override {
				return std::make_shared<Return_T>(begin);
			}
		};

		using ident = pass<ident_pattern>;

		class enclose : public pat_pre_parselet {
		public:
			yshared_ptr<pattern> parse(token const& begin, yparser& parser) override {
				if (auto sub = parser.parse_pattern()) {
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
					throw_expect("pattern");
				}
			}
		};

		class list : public pat_in_parselet {
		public:
			list(ysize prec)
				: pat_in_parselet(prec) {
			}

		public:
			yshared_ptr<pattern> parse(
				yshared_ptr<pattern> left, token const& begin, yparser& parser) override {
				auto ls = std::make_shared<list_pattern>(left);
				do {
					if (auto rhs = parser.parse_pattern(precedence() - 1)) {
						ls->add(rhs);
					}
					else {
						throw_expect("pattern");
					}
				} while (parser.match(ytoken_t::Comma));
				return ls;
			}
		};

		template <bool Right>
		class bin : public pat_in_parselet {
		public:
			bin(ysize prec)
				: pat_in_parselet(prec) {
			}

		public:
			yshared_ptr<pattern> parse(
				yshared_ptr<pattern> left, token const& begin, yparser& parser) override {
				if (auto rhs = parser.parse_expr(precedence() - (Right ? 1 : 0))) {
					return std::make_shared<bin_pattern>(begin, left, rhs);
				}
				else {
					throw_expect("pattern");
				}
			}
		};

		using lbinop = bin<false>;
		using rbinop = bin<true>;
	}
}
