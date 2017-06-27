#include "parselet.h"
#include "parser.h"

namespace parselet {
	void error(parser& p, ystr const& what) {
		auto ahead = p.peek();
		throw parser_expect_exception(p.file(), ahead.Pos, what, ahead.fmt());
	}

	void error(parser& p, ystr const& what, interval const& from) {
		auto ahead = p.peek();
		throw parser_expect_exception(p.file(), ahead.Pos, what, ahead.fmt(), from);
	}

	yopt<token> get_label(parser& p) {
		if (auto beg = term<token_t::LBracket>(p)) {
			if (auto ident = term<token_t::Ident>(p)) {
				if (auto end = term<token_t::RBracket>(p)) {
					return ident;
				}
				else {
					error(p, "']'", beg->Pos);
					return {};
				}
			}
			else {
				error(p, "identifier");
				return {};
			}
		}
		return {};
	}

	AST_expr* get_expr(parser& p, ysize prec) {
		return p.parse_expr(prec);
	}

	AST_ty* get_ty(parser& p, ysize prec) {
		return p.parse_ty(prec);
	}

	AST_stmt* get_stmt(parser& p) {
		return p.parse_stmt();
	}

	yopt<param_t> get_param(parser& p) {
		auto name = term<token_t::Ident>(p);
		auto colon = term<token_t::Colon>(p);
		if (!colon) {
			if (name) {
				error(p, "':'");
				return {};
			}
			else {
				return {};
			}
		}
		auto ty = get_ty_p<0>(p);
		if (!ty) {
			error(p, "parameter type");
			return {};
		}
		return param_t{ name, ty };
	}

	AST_expr* get_func(parser& p, token const& beg) {
		static auto get_param_list =
			make_sep_seq<param_t, token_t::Comma>(get_param, 
				"parameter after ','");

		auto name = get_label(p);

		// For now, if we have parameters, we need parenthesis
		yvec<param_t> params;
		auto lpar = term<token_t::LParen>(p);
		if (lpar) {
			// Parameters
			params = get_param_list(p);
			if (!term<token_t::RParen>(p)) {
				error(p, "')'", lpar->Pos);
			}
			// TODO: if params.size() == 0 => Note: no need for ()
		}
		auto rett = get_ty_not(p);
		auto body = get_body(p);
		if (!body) {
			error(p, "function body");
			return nullptr;
		}
		return new AST_func_expr(beg, name, params, rett, body);
	}

	AST_ty* get_ty_not(parser& p) {
		if (!term<token_t::Arrow>(p)) {
			return nullptr;
		}
		auto ty = get_ty_p<0>(p);
		if (!ty) {
			error(p, "return type after '->'");
			return nullptr;
		}
		return ty;
	}

	AST_body_expr* get_body(parser& p) {
		// TODO: capture block
		auto lbr = term<token_t::LBrace>(p);
		if (!lbr) {
			return nullptr;
		}
		yvec<AST_stmt*> stmts;
		while (auto st = get_stmt(p)) {
			stmts.push_back(st);
		}
		auto exp = get_expr_p<0>(p);
		auto rbr = term<token_t::RBrace>(p);
		if (!rbr) {
			error(p, "'}'", lbr->Pos);
			return nullptr;
		}
		return new AST_body_expr(*lbr, *rbr, stmts, exp);
	}
}
