#include <iostream>
#include "ast.h"
#include "ast_ty.h"
#include "ast_expr.h"
#include "parser.h"
#include "token.h"

namespace parser {
	using namespace combinator;

	namespace {
		const auto Eof = terminal<token_t::EndOfFile>("end-of-file");
		const auto Ident = terminal<token_t::Ident>("identifier");
		const auto Fn = terminal<token_t::Fn>("'fn'");
		const auto Colon = terminal<token_t::Colon>("':'");
		const auto Comma = terminal<token_t::Comma>("','");
		const auto LParen = terminal<token_t::LParen>("'('");
		const auto RParen = terminal<token_t::RParen>("')'");
		const auto Asgn = terminal<token_t::Asgn>("'='");
		const auto LBrace = terminal<token_t::LBrace>("'{'");
		const auto RBrace = terminal<token_t::RBrace>("'}'");
		const auto Arrow = terminal<token_t::Arrow>("'->'");

		const parser_t<AST_ty*> Type = parse_type;
		const parser_t<AST_stmt*> Stmt = parse_stmt;
		const parser_t<AST_expr*> Expr = parse_expr;
		const parser_t<AST_block_expr*> Block = parse_block_expr;
	}

	result_t<token> parse_eof(token_input& in) {
		return Eof(in);
	}

	result_t<AST_ty*> parse_type(token_input& in) {
		static auto TypeParser =
			Ident ^ [](token const& id) -> AST_ty* { return new AST_ident_ty(id); };

		return TypeParser(in);
	}

	result_t<AST_expr*> parse_fn_expr(token_input& in) {
		static auto Param =
			&Ident < Colon >= !Type;

		static auto ParamList =
			(LParen >= &(Param >= *(Comma > !Param)) >= !RParen)
			^ [](auto& lparen, auto& res, auto& rparen) {
				if (res) {
					auto& res_val = res.value();
					auto param0 = make_result_list(
						std::get<0>(res_val),
						std::get<1>(res_val));
					auto& vec = std::get<2>(res_val);
					vec.insert(vec.begin(), param0);
					return std::make_tuple(lparen, vec, rparen);
				}
				else {
					return std::make_tuple(
						lparen,
						yvec<result_list<yopt<token>, AST_ty*>>{},
						rparen);
				}
			};

		static auto FnParser =
			(&ParamList >= &(Arrow > !Type) >= Block)
			^ [](auto& params, auto& rett, auto& block) {
				return new AST_fn_expr();
			};

		//return FnParser(in);
		return std::make_tuple(((AST_expr*)nullptr), in);
	}

	result_t<AST_block_expr*> parse_block_expr(token_input& in) {
		static auto BlockParser =
			(LBrace >= *(Stmt) >= &Expr >= !RBrace)
			^ [](auto& lbr, auto& stmt_list, auto& val, auto& rbr) -> AST_block_expr* {
				return new AST_block_expr(lbr, stmt_list, val, rbr);
			};

		return BlockParser(in);
	}
}
