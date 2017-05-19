#pragma once

#include "yparser.h"
#include "gen_rules.h"
#include "../ast/ast.h"

namespace yk {
	namespace expr_rules {
		using expr_pre_parselet = prefix_parselet<expr, yparser>;
		using expr_in_parselet	= infix_parselet<expr,	yparser>;

		using ident		= gen_rules::pass<expr, ident_expr>;
		using int_lit	= gen_rules::pass<expr, int_lit_expr>;
		using real_lit	= gen_rules::pass<expr, real_lit_expr>;

		class enclose : public expr_pre_parselet {
		public:
			yopt<return_t> parse(token const& lpar, yparser& par) override {
				yvec<param_t> params;
				if (par.same(ytoken_t::Colon)
				 || par.same(ytoken_t::Ident, ytoken_t::Colon)) {
					do {
						yopt<token> id = par.match(ytoken_t::Ident);
						if (par.match(ytoken_t::Colon)) {
							if (auto ty = par.parse_ty_expr(1)) {
								params.push_back({ id, *ty });
							}
							else {
								expect_error("type", "", par);
							}
						}
						else if (id) {
							expect_error("':'", "", par);
						}
					} while (par.match(ytoken_t::Comma));
				}
				if (auto rpar = par.match(ytoken_t::Rpar)) {
					yopt<ty_expr>	ret_t	= {};
					yopt<expr>		body	= {};
					if (auto arr = par.match(ytoken_t::Arrow)) {
						if (!(ret_t = par.parse_ty_expr())) {
							expect_error("return type", "", par);
						}
					}
					if (auto lbr = par.match(ytoken_t::Lbrace)) {
						if (!(body = par.parse_block(*lbr))) {
							expect_error("function body", "", par);
						}
					}
					if (body) {
						return expr(
							lpar.Position * body->Position,
							std::make_shared<fn_expr>(
								params, ret_t, *body));
					}
					if (ret_t) {
						return expr(
							lpar.Position * ret_t->Position,
							std::make_shared<fnproto_expr>(
								params, *ret_t));
					}
					if (params.size()) {
						expect_error("explicit return type for function declaration", "", par);
					}
					return expr(
						lpar.Position * rpar->Position,
						std::make_shared<unit_expr>());
				}
				if (auto sub = par.parse_expr()) {
					if (auto rpar = par.match(ytoken_t::Rpar)) {
						// Extend position
						sub->Position = lpar.Position * rpar->Position;
						return sub;
					}
					expect_error("')'", "", par);
				}
				expect_error("expression", "", par);
			}
		};

		class block : public expr_pre_parselet {
		public:
			yopt<expr> parse(token const& begin, yparser& par) override {
				return par.parse_block(begin);
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
			yopt<expr> parse(token const& op, yparser& par) override {
				if (auto right = par.parse_expr(m_Precedence)) {
					return expr(
						op.Position * right->Position,
						std::make_shared<preury_expr>(op, *right));
				}
				expect_error("right-hand side operand", "", par);
			}
		};

		class let : public expr_pre_parselet {
		public:
			yopt<expr> parse(token const& lett, yparser& par) override {
				if (auto pat = par.parse_pat_expr()) {
					yopt<ty_expr>	ty	= {};
					yopt<expr>		val	= {};
					if (par.match(ytoken_t::Colon)) {
						if (!(ty = par.parse_ty_expr())) {
							expect_error("type", "", par);
						}
					}
					if (par.match(ytoken_t::Asgn)) {
						if (!(val = par.parse_expr())) {
							expect_error("expression", "", par);
						}
					}
					return expr(
						lett.Position * (val ? val->Position :
						(ty ? ty->Position : pat->Position)),
						std::make_shared<let_expr>(*pat, ty, val));
				}
				expect_error("pattern", "", par);
			}
		};

		class postury : public expr_in_parselet {
		public:
			postury(ysize prec)
				: expr_in_parselet(prec) {
			}

		public:
			yopt<expr> parse(expr const& left, token const& op, yparser& par) override {
				return expr(
					left.Position * op.Position,
					std::make_shared<postury_expr>(op, left));
			}
		};

		class list : public expr_in_parselet {
		public:
			list(ysize prec)
				: expr_in_parselet(prec) {
			}

		public:
			yopt<expr> parse(expr const& left, token const& comt, yparser& par) override {
				yvec<expr> elems{ left };
				do {
					if (auto rhs = par.parse_expr(precedence() + 1)) {
						elems.push_back(*rhs);
					}
					else {
						expect_error("expression", "", par);
					}
				} while (par.match(ytoken_t::Comma));
				return expr(
					elems.begin()->Position * elems.rbegin()->Position,
					std::make_shared<list_expr>(elems)
				);
			}
		};

		class call : public expr_in_parselet {
		public:
			call(ysize prec)
				: expr_in_parselet(prec) {
			}

		public:
			yopt<expr> parse(expr const& left, token const& lpar, yparser& par) override {
				auto args = par.parse_expr();
				if (auto rpar = par.match(ytoken_t::Rpar)) {
					return expr(
						left.Position * rpar->Position,
						std::make_shared<call_expr>(left, args));
				}
				expect_error("')'", "", par);
			}

			bool matches(expr const& left, yparser& parser) override {
				return parser.last().Type != ytoken_t::Rbrace;
			}
		};

		template <bool Right, typename Return_T>
		class bin : public expr_in_parselet {
		public:
			bin(ysize prec)
				: expr_in_parselet(prec) {
			}

		public:
			yopt<expr> parse(expr const& left, token const& op, yparser& par) override {
				if (auto rhs = par.parse_expr(precedence() - (Right ? 1 : 0))) {
					return expr(
						left.Position * rhs->Position,
						std::make_shared<Return_T>(op, left, *rhs));
				}
				expect_error("expression", "", par);
			}
		};

		class const_asgn : public bin<true, const_asgn_expr> {
		public:
			const_asgn(ysize prec)
				: bin(prec) {
			}

		public:
			bool matches(expr const& left, yparser& par) override {
				return std::get_if<ysptr<ident_expr>>(&left.Data);
			}
		};

		using lbinop	= bin<false, binop_expr>;
		using rbinop	= bin<true, binop_expr>;
		using asgn		= bin<true, asgn_expr>;
	}
}
