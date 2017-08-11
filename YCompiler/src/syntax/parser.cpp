#include <iostream>
#include "ast.h"
#include "ast_ty.h"
#include "ast_expr.h"
#include "ast_stmt.h"
#include "parser.h"
#include "token.h"

namespace parser {
	namespace {
		const auto Ident = terminal<token_t::Ident>("identifier");
		const auto LParen = terminal<token_t::LParen>("'('");
		const auto RParen = terminal<token_t::RParen>("')'");
		const auto LBrace = terminal<token_t::LBrace>("'{'");
		const auto RBrace = terminal<token_t::RBrace>("'}'");
		const auto Colon = terminal<token_t::Colon>("':'");
		const auto Comma = terminal<token_t::Comma>("','");
		const auto Arrow = terminal<token_t::Arrow>("'->'");
		const auto Fn = terminal<token_t::Fn>("'fn'");
		const auto Asgn = terminal<token_t::Asgn>("'='");

		parser_t<AST_ty*> Type = parse_type;
		parser_t<AST_stmt*> Stmt = parse_stmt;
		parser_t<AST_expr*> Expr = parse_expr;

		parser_t<AST_block_expr*> Block = parse_block_expr;
		parser_t<AST_decl_stmt*> Decl = parse_decl_stmt;
		parser_t<AST_fn_expr*> FnExpr = parse_fn_expr;
	}

	result_t<AST_ty*> parse_type(token_input& in) {
		static auto type_parser =
			Ident ^ [](auto& tok) -> AST_ty* { return new AST_ident_ty(tok); };

		return type_parser(in);
	}

	result_t<AST_fn_expr*> parse_fn_expr(token_input& in) {
		yopt<token> first_tok = {};
		static auto Param =
			&Ident < Colon >= !Type;
		static auto ParamList =
			(LParen >= &(Param >= *(Comma > !Param)) >= !RParen)
			^ [&](auto& lp, auto& params, auto& rp) {
				if (params) {
					auto& res_val = params.value();
					auto param0 = make_result_list(
						std::get<0>(res_val),
						std::get<1>(res_val));
					auto& vec = std::get<2>(res_val);
					vec.insert(vec.begin(), param0);
					return std::make_tuple(lp, vec, rp);
				}
				else {
					return std::make_tuple(
						lp,
						yvec<result_list<yopt<token>, AST_ty*>>{},
						rp);
				}
			};
		static auto fn_parser =
			(&ParamList >= &(Arrow >= !Type) >= Block)
			^ [](auto& params, auto& rett, auto& body) -> AST_fn_expr* {
				yopt<token> beg = {};
				yopt<AST_ty*> rett_r = {};
				yvec<AST_fn_expr::param_t> param_ls;
				if (params) {
					beg = std::get<0>(*params);
					auto& params_ok = std::get<1>(*params);
					for (auto& par : params_ok) {
						param_ls.push_back(par.as_tuple());
					}
				}
				else if (rett) {
					beg = std::get<0>(*rett);
					rett_r = std::get<1>(*rett);
				}
				return new AST_fn_expr(beg, param_ls, rett_r, body);
			};

		return fn_parser(in);
	}

	result_t<AST_block_expr*> parse_block_expr(token_input& in) {
		static auto block_parser =
			(LBrace >= *(Stmt) >= &Expr >= !RBrace)
			^ [](auto& lbr, auto& stmt_list, auto& val, auto& rbr) -> AST_block_expr* {
				return new AST_block_expr(lbr, stmt_list, val, rbr);
			};

		return block_parser(in);
	}

	result_t<AST_stmt*> parse_stmt(token_input& in) {
		static auto stmt_parser =
			Ident ^ [](auto& in) -> AST_stmt* { return nullptr; };

		return stmt_parser(in);
	}

	result_t<AST_expr*> parse_expr(token_input& in) {
		static auto expr_parser =
			Ident ^ [](auto& in) -> AST_expr* { return nullptr; };

		return expr_parser(in);
	}

	result_t<AST_decl_stmt*> parse_decl_stmt(token_input& in) {
		static auto decl_parser =
			(Fn >= !Ident < !Asgn >= FnExpr)
			^ [](auto& beg, auto& id, auto& fn) -> AST_decl_stmt* {
				return new AST_decl_stmt(beg, id, fn);
			};

		return decl_parser(in);
	}
}
