#pragma once

#include "common_rules.h"
#include "yparser.h"
#include "../ast/pattern.h"

namespace yk {
	namespace pat_rules {
		using pat_pre_parselet	= prefix_parselet<pattern, yparser>;
		using pat_in_parselet	= infix_parselet<pattern, yparser>;

		using ident		= common_rules::pass<pat_pre_parselet, ident_pattern>;
		using ignore	= common_rules::pass<pat_pre_parselet, ignore_pattern>;

		class enclose : public pat_pre_parselet {
		public:
			ysptr<pattern> parse(token const& begin, yparser& par) override {
				if (auto sub = par.parse_pattern()) {
					if (par.match(ytoken_t::Rpar)) {
						return sub;
					}
					else {
						token const& tok = par.peek();
						expect_error("')'", "", par);
					}
				}
				else {
					if (auto end = par.match(ytoken_t::Rpar)) {
						return std::make_shared<unit_pattern>(begin, end.value());
					}
					else {
						token const& tok = par.peek();
						expect_error("pattern", "", par);
					}
				}
			}
		};

		class list : public pat_in_parselet {
		public:
			list(ysize prec)
				: pat_in_parselet(prec) {
			}

		public:
			ysptr<pattern> parse(
				ysptr<pattern> left, token const& begin, yparser& par) override {
				auto ls = std::make_shared<list_pattern>(left);
				do {
					if (auto rhs = par.parse_pattern(precedence() + 1)) {
						ls->add(rhs);
					}
					else {
						expect_error("pattern", "", par);
					}
				} while (par.match(ytoken_t::Comma));
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
			ysptr<pattern> parse(
				ysptr<pattern> left, token const& begin, yparser& par) override {
				if (auto rhs = parser.parse_expr(precedence() - (Right ? 1 : 0))) {
					return std::make_shared<bin_pattern>(begin, left, rhs);
				}
				else {
					expect_error("pattern", "", par);
				}
			}
		};

		using lbinop = bin<false>;
		using rbinop = bin<true>;
	}
}
