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
			ysptr<pattern> parse(token const& begin, yparser& par) override {
				return std::make_shared<Return_T>(begin);
			}
		};

		using ident = pass<ident_pattern>;

		class enclose : public pat_pre_parselet {
		public:
			ysptr<pattern> parse(token const& begin, yparser& par) override {
				if (auto sub = par.parse_pattern()) {
					if (par.match(ytoken_t::Rpar)) {
						return sub;
					}
					else {
						token const& tok = par.peek();
						throw_expect("')'", par);
					}
				}
				else {
					token const& tok = par.peek();
					throw_expect("pattern", par);
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
					if (auto rhs = par.parse_pattern(precedence() - 1)) {
						ls->add(rhs);
					}
					else {
						throw_expect("pattern", par);
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
					throw_expect("pattern", par);
				}
			}
		};

		using lbinop = bin<false>;
		using rbinop = bin<true>;
	}
}
