#pragma once

#include "gen_rules.h"
#include "yparser.h"
#include "../ast/ast.h"

namespace yk {
	namespace pat_rules {
		using pat_pre_parselet	= prefix_parselet<pat_expr, yparser>;
		using pat_in_parselet	= infix_parselet<pat_expr, yparser>;

		using ident		= gen_rules::pass<pat_expr, ident_pat_expr>;
		using ignore	= gen_rules::pass_ign<pat_expr, ignore_pat_expr>;

		class enclose : public pat_pre_parselet {
		public:
			yopt<pat_expr> parse(token const& lpar, yparser& par) override {
				if (auto sub = par.parse_pat_expr()) {
					if (auto rpar = par.match(ytoken_t::Rpar)) {
						// Extend position
						sub->Position = lpar.Position * rpar->Position;
						return sub;
					}
					else {
						expect_error("')'", par);
					}
				}
				if (auto rpar = par.match(ytoken_t::Rpar)) {
					return pat_expr(
						lpar.Position * rpar->Position,
						std::make_shared<unit_pat_expr>());
				}
				expect_error("pattern", par);
			}
		};

		class list : public pat_in_parselet {
		public:
			list(ysize prec)
				: pat_in_parselet(prec) {
			}

		public:
			yopt<pat_expr> parse(pat_expr const& left, token const& comt, yparser& par) override {
				yvec<pat_expr> elems{ left };
				do {
					if (auto rhs = par.parse_pat_expr(precedence() + 1)) {
						elems.push_back(*rhs);
					}
					else {
						expect_error("pattern", par);
					}
				} while (par.match(ytoken_t::Comma));
				return pat_expr(
					elems.begin()->Position * elems.rbegin()->Position,
					std::make_shared<list_pat_expr>(elems));
			}
		};
	}
}
