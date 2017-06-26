#include "parselet_common.h"
#include "parselet_expr.h"
#include "parselet_stmt.h"
#include "parser.h"
#include "parselet_ty.h"

namespace parselet_expr {
	AST_expr* get_expr(parser& p, ysize prec) {
		return p.parse_expr(prec);
	}

	AST_body_expr* get_body(parser& p) {
		// TODO: capture block
		using namespace parselet_common;
		using namespace parselet_stmt;
		using namespace std::placeholders;
		if (auto lbr = opt(get_token<token_t::LBrace>)(p)) {
			yvec<AST_stmt*> body;
			while (auto st = opt(get_stmt)(p)) {
				body.push_back(st);
			}
			auto exp = opt(std::bind(get_expr, _1, 0))(p);
			auto rbr = req<yopt<token>>(get_token<token_t::RBrace>, "'}'", lbr->Pos)(p);
			return new AST_body_expr(*lbr, *rbr, body, exp);
		}
		return nullptr;
	}

	yopt<param_t> get_param(parser& p) {
		using namespace parselet_common;
		using namespace parselet_ty;

		auto ident = get_token<token_t::Ident>(p);
		if (!get_token<token_t::Colon>(p)) {
			if (ident) {
				parse_error(p, "':'");
			}
			return {};
		}
		AST_ty* ty = req<AST_ty*>(get_ty, "type")(p);
		return param_t({ ident, ty });
	}

	AST_expr* get_func(parser& p, token const& beg) {
		using namespace parselet_common;

		yopt<token> ident = get_label(p);
		// Params
		auto lpar = get_token<token_t::LParen>(p);

		yvec<param_t> params;
		{
			if (auto par = get_param(p)) {
				params.push_back(*par);
				while (get_token<token_t::Comma>(p)) {
					if (auto par2 = get_param(p)) {
						params.push_back(*par2);
					}
					else {
						parse_error(p, "parameter");
						return {};
					}
				}
			}
		}

		if (lpar) {
			token rpar = p.peek();
			if (rpar.Type != token_t::RParen) {
				parse_error(p, "')'", lpar->Pos);
				return nullptr;
			}
			p.consume();
		}

		// Return type
		AST_ty* rett = nullptr;
		if (get_token<token_t::Arrow>(p)) {
			if (!(rett = p.parse_ty())) {
				parse_error(p, "return type");
				return nullptr;
			}
		}

		// Body
		AST_body_expr* body = get_body(p);
		if (!body) {
			token beg = p.peek();
			parse_error(p, "body");
			return nullptr;
		}
		return new AST_func_expr(beg, ident, params, rett, body);
	}
}
