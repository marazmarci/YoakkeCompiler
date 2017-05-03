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
			ysptr<expr> parse(token const& begin, yparser& par) override {
				if (auto rpar = par.match(ytoken_t::Rpar)) {
					// ()
					if (par.match(ytoken_t::Arrow)) {
						// () ->
						if (auto rett = par.parse_type_desc()) {
							if (auto lbrace = par.match(ytoken_t::Lbrace)) {
								return std::make_shared<fn_expr>
									(begin, yvec<fn_expr::param_t>{},
										rett,
										par.parse_block(lbrace.value()));
							}
							else {
								auto proto = std::make_shared<fnproto_expr>(
									begin, *rpar, yvec<fnproto_expr::param_t>{}, rett);
							}
						}
						else {
							expect_error("return type", "", par);
						}
					}
					else if (auto lbrace = par.match(ytoken_t::Lbrace)) {
						// () {}
						return std::make_shared<fn_expr>(begin,
							yvec<fn_expr::param_t>{},
							nullptr,
							par.parse_block(lbrace.value()));
					}
					else {
						return std::make_shared<unit_expr>(begin, rpar.value());
					}
				}
				else if ((par.peek().Type	== ytoken_t::Colon)
						|| (par.peek().Type == ytoken_t::Ident
						&& par.peek(1).Type == ytoken_t::Colon)) {
					// (<ident>:
					yvec<fnproto_expr::param_t> params;
					do {
						yopt<token> id = par.match(ytoken_t::Ident);
						if (par.match(ytoken_t::Colon)) {
							if (auto type = par.parse_type_desc(1)) {
								params.push_back({ id, type });
							}
							else {
								expect_error("type", "", par);
							}
						}
						else if (id) {
							expect_error("':'", "", par);
						}
					} while (par.match(ytoken_t::Comma));
					// (<ident>: ...)
					if (auto rpar = par.match(ytoken_t::Rpar)) {
						ysptr<ty_expr> rett = nullptr;
						if (par.match(ytoken_t::Arrow)) {
							// (<ident>: ...) ->
							if (rett = par.parse_type_desc()) {
							}
							else {
								expect_error("'->'", "", par);
							}
						}
						if (auto lbrace = par.match(ytoken_t::Lbrace)) {
							if (auto body = par.parse_block(lbrace.value())) {
								return std::make_shared<fn_expr>(begin,
									params, rett, body);
							}
							else {
								return nullptr;
							}
						}
						else if (rett) {
							return std::make_shared<fnproto_expr>
								(begin, *rpar, params, rett);
						}
						else {
							expect_error("explicit return type for function declaration", "", par);
						}
					}
					else {
						expect_error("')'", "", par);
					}
				}
				else {
					if (auto expr = par.parse_expr()) {
						if (auto rpar = par.match(ytoken_t::Rpar)) {
							return expr;
						}
						else {
							expect_error("')'", "", par);
						}
					}
					else {
						expect_error("expression", "", par);
					}
				}
			}
		};

		class block : public expr_pre_parselet {
		public:
			ysptr<expr> parse(token const& begin, yparser& par) override {
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
			ysptr<expr> parse(token const& begin, yparser& par) override {
				if (auto right = par.parse_expr(m_Precedence)) {
					return std::make_shared<preury_expr>(begin, right);
				}
				else {
					expect_error("right-hand side operand", "", par);
				}
			}
		};

		class let : public expr_pre_parselet {
		public:
			ysptr<expr> parse(token const& begin, yparser& par) override {
				if (auto pat = par.parse_pattern()) {
					ysptr<ty_expr> type = nullptr;
					if (par.match(ytoken_t::Colon)) {
						if (type = par.parse_type_desc()) {
						}
						else {
							expect_error("type", "", par);
						}
					}
					ysptr<expr> val = nullptr;
					if (par.match(ytoken_t::Asgn)) {
						if (val = par.parse_expr()) {
						}
						else {
							expect_error("expression", "", par);
						}
					}
					return std::make_shared<let_expr>(pat, type, val, begin);
				}
				else {
					expect_error("pattern", "", par);
				}
			}
		};

		class postury : public expr_in_parselet {
		public:
			postury(ysize prec)
				: expr_in_parselet(prec) {
			}

		public:
			ysptr<expr> parse(
				ysptr<expr> left, token const& begin, yparser& par) override {
				return std::make_shared<postury_expr>(begin, left);
			}
		};

		class list : public expr_in_parselet {
		public:
			list(ysize prec)
				: expr_in_parselet(prec) {
			}

		public:
			ysptr<expr> parse(
				ysptr<expr> left, token const& begin, yparser& par) override {
				auto ls = std::make_shared<list_expr>(left);
				do {
					if (auto rhs = par.parse_expr(precedence() + 1)) {
						ls->add(rhs);
					}
					else {
						expect_error("expression", "", par);
					}
				} while (par.match(ytoken_t::Comma));
				return ls;
			}
		};

		class call : public expr_in_parselet {
		public:
			call(ysize prec)
				: expr_in_parselet(prec) {
			}

		public:
			ysptr<expr> parse(
				ysptr<expr> left, token const& begin, yparser& par) override {
				auto args = par.parse_expr();
				if (auto tok = par.match(ytoken_t::Rpar)) {
					return std::make_shared<call_expr>(left, args, tok.value());
				}
				else {
					expect_error("')'", "", par);
				}
			}

			bool matches(ysptr<expr> left, yparser& parser) override {
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
			ysptr<expr> parse(
				ysptr<expr> left, token const& begin, yparser& par) override {
				if (auto rhs = par.parse_expr(precedence() - (Right ? 1 : 0))) {
					return std::make_shared<Return_T>(begin, left, rhs);
				}
				else {
					expect_error("expression", "", par);
				}
			}
		};

		class const_asgn : public bin<true, const_asgn_expr> {
		public:
			const_asgn(ysize prec)
				: bin(prec) {
			}

		public:
			bool matches(ysptr<expr> left, yparser& par) override {
				return std::dynamic_pointer_cast<ident_expr>(left) != nullptr;
			}
		};

		using lbinop = bin<false, binop_expr>;
		using rbinop = bin<true, binop_expr>;
		using asgn = bin<true, asgn_expr>;
	}
}
