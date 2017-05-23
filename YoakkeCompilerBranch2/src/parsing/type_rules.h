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
			yopt<ty_expr> parse(token const& lpar, yparser& par) override {
				if (auto sub = par.parse_ty_expr()) {
					if (auto rpar = par.match(ytoken_t::Rpar)) {
						// Extend position
						sub->Position = lpar.Position * rpar->Position;
						return sub;
					}
					expect_error("')'", par);
				}
				if (auto rpar = par.match(ytoken_t::Rpar)) {
					return ty_expr(
						lpar.Position * rpar->Position,
						std::make_shared<unit_ty_expr>());
				}
				expect_error("type", par);
			}
		};

		class list : public type_in_parselet {
		public:
			list(ysize prec)
				: type_in_parselet(prec) {
			}

		public:
			yopt<ty_expr> parse(ty_expr const& left, token const& comt, yparser& par) override {
				yvec<ty_expr> elems{ left };
				do {
					if (auto rhs = par.parse_ty_expr(precedence() + 1)) {
						elems.push_back(*rhs);
					}
					else {
						expect_error("type", par);
					}
				} while (par.match(ytoken_t::Comma));
				return ty_expr(
					elems.begin()->Position * elems.rbegin()->Position,
					std::make_shared<list_ty_expr>(elems));
			}
		};

		template <bool Right>
		class bin : public type_in_parselet {
		public:
			bin(ysize prec)
				: type_in_parselet(prec) {
			}

		public:
			yopt<ty_expr> parse(ty_expr const& left, token const& op, yparser& par) override {
				if (auto rhs = par.parse_ty_expr(precedence() - (Right ? 1 : 0))) {
					return ty_expr(
						left.Position * rhs->Position,
						std::make_shared<bin_ty_expr>(op, left, *rhs));
				}		
				expect_error("type", par);
			}
		};

		using lbinop = bin<false>;
		using rbinop = bin<true>;
	}
}
