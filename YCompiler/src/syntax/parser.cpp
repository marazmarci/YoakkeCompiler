#include <iostream>
#include "ast.h"
#include "ast_ty.h"
#include "ast_expr.h"
#include "ast_stmt.h"
#include "parser.h"
#include "token.h"
#include "../functions.h"
#include "../io/fmt_code.h"

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
			&Ident < Colon >= !(Type / "parameter type");
		static auto ParamList =
			(LParen >= &((Param / "parameter") >= *(Comma > !(Param / "parameter"))) >= !RParen)
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
			(&ParamList >= &(Arrow >= !(Type / "return type")) >= Block)
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
			Decl ^ [](auto& res) -> AST_stmt* { return res; };

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

	/*************************************************************************/

	namespace {
		void handle_error_desc(yvar<lexer_err, parser_err>& err) {
			fnl::match(err)(
			[](lexer_err& err) {
				fnl::match(err)(
				[](lexer_eof_err& err) {
					auto& start = err.Start;
					auto& end = err.End;
					std::cout
						<< "Unexpected end of file in file '"
						<< err.File.path() << "' at line "
						<< end.End.Row << ", character "
						<< end.End.Column << "!"
						<< std::endl;
					fmt_code::print(err.File, start, end);
					std::cout << err.Msg << std::endl;
					if (err.Note) {
						std::cout << "Note: " << *err.Note << std::endl;
					}
				},
				[](lexer_unk_tok_err& err) {
					std::cout
						<< "Unknown token '"
						<< err.Tok << "' in file '"
						<< err.File.path() << "' at line "
						<< err.Pos.Start.Row << ", character "
						<< err.Pos.Start.Column << "!"
						<< std::endl;
					fmt_code::print(err.File, err.Pos);
				}
				);
			},
			[](parser_err& err) {
				fnl::match(err)(
				[](parser_exp_tok_err& err) {
					auto& pos = err.Got.Pos;
					std::cout
						<< "Syntax error in file '"
						<< err.File.path() << "' at line "
						<< pos.Start.Row << ", character "
						<< pos.Start.Column << "!"
						<< std::endl;
					fmt_code::print(err.File, pos);
					std::cout
						<< "Expected " << err.Expected
						<< ", but got " << err.Got.fmt()
						<< "." << std::endl;
				}
				);
			}
			);
		}
	}

	void handle_error(fail_info& err) {
		handle_error_desc(err.err());
	}
}
