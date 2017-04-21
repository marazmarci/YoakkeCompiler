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
			ysptr<expr> parse(token const& begin, yparser& par) override {
				return std::make_shared<Return_T>(begin);
			}
		};

		using ident = pass<ident_expr>;
		using int_lit = pass<int_lit_expr>;
		using real_lit = pass<real_lit_expr>;

		class enclose : public expr_pre_parselet {
		public:
			ysptr<expr> parse(token const& begin, yparser& par) override {
				if (auto rpar = par.match(ytoken_t::Rpar)) {
					// ()
					if (par.match(ytoken_t::Arrow)) {
						// () ->
						if (auto rett = par.parse_type_desc()) {
							auto proto = std::make_shared<fnproto_expr>(
								yvec<fnproto_expr::param_t>{}, rett, begin, rpar.value());
							if (auto lbrace = par.match(ytoken_t::Lbrace)) {
								return std::make_shared<fn_expr>(proto, par.parse_block(lbrace.value()));
							}
							else {
								return proto;
							}
						}
						else {
							throw_expect("return type", par);
						}
					}
					else if (auto lbrace = par.match(ytoken_t::Lbrace)) {
						// () {}
						auto proto = std::make_shared<fnproto_expr>(
							yvec<fnproto_expr::param_t>{}, nullptr, begin, rpar.value());
						return std::make_shared<fn_expr>(proto, par.parse_block(lbrace.value()));
					}
					else {
						return std::make_shared<unit_expr>(begin, rpar.value());
					}
				}
				else if ((par.peek().Type == (ysize)ytoken_t::Colon)
						|| (par.peek().Type == (ysize)ytoken_t::Ident
						&& par.peek(1).Type == (ysize)ytoken_t::Colon)) {
					// (<ident>:
					std::vector<fnproto_expr::param_t> params;
					do {
						yopt<token> id = par.match(ytoken_t::Ident);
						if (par.match(ytoken_t::Colon)) {
							if (auto type = par.parse_type_desc(1)) {
								params.push_back({ id, type });
							}
							else {
								throw_expect("type", par);
							}
						}
						else if (id) {
							throw_expect("':'", par);
						}
					} while (par.match(ytoken_t::Comma));
					// (<ident>: ...)
					if (auto rpar = par.match(ytoken_t::Rpar)) {
						ysptr<type_desc> rett = nullptr;
						if (par.match(ytoken_t::Arrow)) {
							// (<ident>: ...) ->
							if (rett = par.parse_type_desc()) {
							}
							else {
								throw_expect("'->'", par);
							}
						}
						auto proto = std::make_shared<fnproto_expr>(params, rett, begin, rpar.value());
						if (auto lbrace = par.match(ytoken_t::Lbrace)) {
							auto body = par.parse_block(lbrace.value());
							return std::make_shared<fn_expr>(proto, body);
						}
						else if (rett) {
							return proto;
						}
						else {
							throw_expect("explicit return type for function declaration", par);
						}
					}
					else {
						throw_expect("')'", par);
					}
				}
				else {
					if (auto expr = par.parse_expr()) {
						if (auto rpar = par.match(ytoken_t::Rpar)) {
							return expr;
						}
						else {
							throw_expect("')'", par);
						}
					}
					else {
						throw_expect("expression", par);
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
					throw_expect("right-hand side operand", par);
				}
			}
		};

		class let : public expr_pre_parselet {
		public:
			ysptr<expr> parse(token const& begin, yparser& par) override {
				if (auto pat = par.parse_pattern()) {
					ysptr<type_desc> type = nullptr;
					if (par.match(ytoken_t::Colon)) {
						if (type = par.parse_type_desc()) {
						}
						else {
							throw_expect("type", par);
						}
					}
					ysptr<expr> val = nullptr;
					if (par.match(ytoken_t::Asgn)) {
						if (val = par.parse_expr()) {
						}
						else {
							throw_expect("expression", par);
						}
					}
					return std::make_shared<let_expr>(pat, type, val, begin);
				}
				else {
					throw_expect("pattern", par);
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
						throw_expect("expression", par);
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
					throw_expect("')'", par);
				}
			}

			bool matches(ysptr<expr> left, yparser& parser) override {
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
			ysptr<expr> parse(
				ysptr<expr> left, token const& begin, yparser& par) override {
				if (auto rhs = par.parse_expr(precedence() - (Right ? 1 : 0))) {
					return std::make_shared<Return_T>(begin, left, rhs);
				}
				else {
					throw_expect("expression", par);
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