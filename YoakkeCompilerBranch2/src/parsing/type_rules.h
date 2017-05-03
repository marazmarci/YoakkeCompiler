#pragma once

#include "gen_rules.h"
#include "yparser.h"
#include "../ast/ast.h"

namespace yk {
	namespace type_rules {
		using type_pre_parselet = prefix_parselet<ty_expr, yparser>;
		using type_in_parselet = infix_parselet<ty_expr, yparser>;

		using ident = gen_rules::pass<ty_expr, ident_ty_expr>;

		class enclose : public type_pre_parselet {
		public:
			ysptr<ty_expr> parse(token const& begin, yparser& par) override {
				if (auto sub = par.parse_type_desc()) {
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
						return std::make_shared<unit_ty_expr>(begin, end.value());
					}
					else {
						token const& tok = par.peek();
						expect_error("type", "", par);
					}
				}
			}
		};

		class list : public type_in_parselet {
		public:
			list(ysize prec)
				: type_in_parselet(prec) {
			}

		public:
			ysptr<ty_expr> parse(
				ysptr<ty_expr> left, token const& begin, yparser& par) override {
				auto ls = std::make_shared<list_ty_expr>(left);
				do {
					if (auto rhs = par.parse_type_desc(precedence() + 1)) {
						ls->add(rhs);
					}
					else {
						expect_error("type", "", par);
					}
				} while (par.match(ytoken_t::Comma));
				return ls;
			}
		};

		template <bool Right>
		class bin : public type_in_parselet {
		public:
			bin(ysize prec)
				: type_in_parselet(prec) {
			}

		public:
			ysptr<ty_expr> parse(
				ysptr<ty_expr> left, token const& begin, yparser& par) override {
				if (auto rhs = par.parse_type_desc(precedence() - (Right ? 1 : 0))) {
					return std::make_shared<bin_ty_expr>(begin, left, rhs);
				}
				else {
					expect_error("type", "", par);
				}
			}
		};

		using lbinop = bin<false>;
		using rbinop = bin<true>;
	}
}
