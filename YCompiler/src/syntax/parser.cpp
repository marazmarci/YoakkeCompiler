#include <cassert>
#include <iostream>
#include "ast.h"
#include "ast_ty.h"
#include "ast_expr.h"
#include "ast_stmt.h"
#include "ast_pat.h"
#include "parser.h"
#include "token.h"
#include "../functions.h"
#include "../io/fmt_code.h"

namespace parser {
	namespace {
		const auto IDENT	= term<token_t::Ident>();
		const auto LPAREN	= term<token_t::LParen>();	// '('
		const auto RPAREN	= term<token_t::RParen>();	// ')'
		const auto LBRACE	= term<token_t::LBrace>();	// '{'
		const auto RBRACE	= term<token_t::RBrace>();	// '}'
		const auto COLON	= term<token_t::Colon>();	// ':'
		const auto COMMA	= term<token_t::Comma>();	// ','
		const auto ARROW	= term<token_t::Arrow>();	// '->'
		const auto PLUS		= term<token_t::Add>();		// '+'
		const auto MINUS	= term<token_t::Sub>();		// '-'
		const auto STAR		= term<token_t::Mul>();		// '*'
		const auto SLASH	= term<token_t::Div>();		// '/'
		const auto PERCENT	= term<token_t::Mod>();		// '%'
		const auto GREATER	= term<token_t::Greater>();	// '>'
		const auto LESS		= term<token_t::Less>();	// '<'
		const auto GREQ		= term<token_t::GrEq>();	// '>='
		const auto LEEQ		= term<token_t::LeEq>();	// '<='
		const auto EQ		= term<token_t::Eq>();		// '=='
		const auto NEQ		= term<token_t::Neq>();		// '<>'
		const auto ASGN		= term<token_t::Asgn>();	// '='
		const auto FN		= term<token_t::Fn>();		// 'fn'
		const auto SEMICOL	= term<token_t::Semicol>();	// ';'
		const auto LET		= term<token_t::Let>();		// 'let'
		const auto IF		= term<token_t::If>();		// 'if'
		const auto ELSE		= term<token_t::Else>();	// 'else'
		const auto TYPE		= term<token_t::Type>();	// 'type'
		const auto TRUE		= term<token_t::True>();	// 'true'
		const auto FALSE	= term<token_t::False>();	// 'false'
		const auto INT_LIT	= term<token_t::IntLit>();
		const auto REAL_LIT = term<token_t::RealLit>();
		const auto END_OF_F = term<token_t::EndOfFile>();

		const auto DBG_WR_T = term<token_t::DbgWriteTy>();
	}

	namespace {
		template <typename T>
		auto cast() {
			return [](auto& param) -> T* {
				return param;
			};
		}

		template <typename T, typename... Ps>
		auto make(Ps&... params) {
			return [](auto&... params) {
				return new T(params...);
			};
		}

		template <typename T, typename U>
		auto make_as() {
			return [](auto&... params) -> U* {
				return new T(params...);
			};
		}

		template <typename TC>
		auto foldl_bintree() {
			return [](auto& left, auto& rights) {
				return fnl::fold(rights.begin(), rights.end(), left,
					[](auto& left, auto& rhs) -> auto {
					auto& op = std::get<0>(rhs);
					auto& right = std::get<1>(rhs);
					return new TC(left, op, right);
				});
			};
		};

		template <typename TC>
		auto foldr_bintree() {
			return [](auto& left, auto& rights) {
				using ret_t = std::decay_t<decltype(left)>;
				if (rights.size()) {
					auto it = rights.rbegin();
					auto op = std::get<0>(*it);
					auto curr = std::get<1>(*it);
					it++;
					while (it != rights.rend()) {
						auto& tup = *it;
						auto right = std::get<1>(tup);
						curr = new TC(right, op, curr);
						op = std::get<0>(tup);
						it++;
					}
					return (ret_t)(new TC(left, op, curr));
				}
				else {
					return left;
				}
			};
		};

		template <typename TC, typename T>
		auto list_or_single() {
			return [](auto& first, auto& rest) -> T* {
				if (rest.size()) {
					rest.insert(rest.begin(), first);
					return new TC(rest);
				}
				else {
					return first;
				}
			};
		}

		result_t<AST_stmt*> parse_stmt(token_input& in);
		result_t<AST_stmt*> parse_decl(token_input& in);
		
		const parser_t<AST_stmt*> Stmt = parse_stmt;
		const parser_t<AST_stmt*> Decl = parse_decl;

		/*********************************************************************/

		namespace type_detail {
			result_t<AST_ty*> parse_ty(token_input& in);
			result_t<AST_ty*> parse_ty_list(token_input& in);

			const parser_t<AST_ty*> Type = parse_ty;
			const parser_t<AST_ty*> TypeList = parse_ty_list;

			const auto lvl0 =
				  (IDENT ^ make_as<AST_ident_ty, AST_ty>())
				| ((LPAREN >= RPAREN) ^ make_as<AST_list_ty, AST_ty>())
				| (LPAREN > TypeList < !RPAREN)
				;

			const auto lvl1 =
				(lvl0 >= *(ARROW >= !lvl0)) ^ foldr_bintree<AST_bin_ty>();
				;

			const auto lvl_list =
				(Type >= *(COMMA > !Type)) ^ list_or_single<AST_list_ty, AST_ty>()
				;

			result_t<AST_ty*> parse_ty(token_input& in) {
				return lvl1(in);
			}

			result_t<AST_ty*> parse_ty_list(token_input& in) {
				return lvl_list(in);
			}
		}

		const auto Type = type_detail::Type;
		const auto TypeList = type_detail::TypeList;

		/*********************************************************************/

		namespace pattern_detail {
			result_t<AST_pat*> parse_pat(token_input& in);

			const parser_t<AST_pat*> Pattern = parse_pat;

			const auto lvl0 =
				  (IDENT ^ make_as<AST_ident_pat, AST_pat>())
				| ((LPAREN >= RPAREN) ^ make_as<AST_list_pat, AST_pat>())
				| (LPAREN > Pattern < !RPAREN)
				;

			const auto lvl1 =
				(lvl0 >= *(COMMA > !lvl0)) ^ list_or_single<AST_list_pat, AST_pat>()
				;

			result_t<AST_pat*> parse_pat(token_input& in) {
				return lvl1(in);
			}
		}

		const auto Pattern = pattern_detail::Pattern;

		/*********************************************************************/

		result_t<AST_block_expr*> parse_block_stmt(token_input& in);

		const parser_t<AST_block_expr*> BlockStmt = parse_block_stmt;

		namespace expr_detail {
			result_t<AST_expr*> parse_expr(token_input& in);
			result_t<AST_expr*> parse_expr_list(token_input& in);

			const parser_t<AST_expr*> Expr = parse_expr;
			const parser_t<AST_expr*> ExprList = parse_expr_list;

			const auto BlockExpr =
				(LBRACE >= *Stmt >= &ExprList >= !RBRACE) ^ make<AST_block_expr>();

			const auto IfExpr =
				(IF >= !(ExprList / "condition") >= !BlockStmt
					>= *(ELSE > IF >= !(ExprList / "condition") >= !BlockStmt)
					>= !((ELSE > BlockStmt) / "else-block"))
				^ [](auto& beg, auto& cond, auto& then, auto& elifs, auto& el) -> AST_if_expr* {
					AST_block_expr* elbody =
						fnl::fold(elifs.rbegin(), elifs.rend(), el,
							[=](auto& curr, auto& elem) -> AST_block_expr* {
								auto res = new AST_if_expr(
									std::get<0>(elem),
									std::get<1>(elem),
									std::get<2>(elem),
									curr);
								res->AsStatement = true;
								return new AST_block_expr(new AST_expr_stmt(res));
							});
					return new AST_if_expr(beg, cond, then, elbody);
				};

			const auto FnParam =
				&IDENT < COLON >= !(Type / "parameter type");

			const auto FnParamList =
				(LPAREN >= &((FnParam / "parameter") >= *(COMMA > !(FnParam / "parameter"))) >= !RPAREN)
				^ [](auto& lp, auto& params, auto& rp) {
					if (params) {
						auto& res_val = params.value();
						auto param0 = make_result_list(
							std::get<0>(res_val), std::get<1>(res_val)
						);
						auto& vec = std::get<2>(res_val);
						vec.insert(vec.begin(), param0);
						return std::make_tuple(lp, vec, rp);
					}
					else {
						return std::make_tuple(
							lp, yvec<result_list<yopt<token>, AST_ty*>>{}, rp
						);
					}
				};

			const auto FnExpr =
				(&FnParamList >= &(ARROW >= !(TypeList / "return type")) >= BlockExpr)
				^ [](auto& params, auto& rett, auto& body) -> AST_fn_expr* {
					yopt<token> beg = {};
					yopt<AST_ty*> rett_r = {};
					yvec<AST_fn_expr::param_t> param_ls;
					body->AsStatement = true;
					if (params) {
						beg = std::get<0>(*params);
						auto& params_ok = std::get<1>(*params);
						for (auto& par : params_ok) {
							param_ls.push_back(par.as_tuple());
						}
						if (rett) {
							rett_r = std::get<1>(*rett);
						}
					}
					else if (rett) {
						beg = std::get<0>(*rett);
						rett_r = std::get<1>(*rett);
					}
					return new AST_fn_expr(beg, param_ls, rett_r, body);
				};

			const auto LetExpr = (LET >= !(Pattern / "pattern")
				>= &(COLON > !(TypeList / "type"))
				>= &(ASGN > !(ExprList / "expression")))
				^ make_as<AST_let_expr, AST_expr>();

			const auto CallParamList =
				Expr >= *(COMMA > !Expr);

			const auto lvl0 =
				  (INT_LIT ^ make_as<AST_int_lit_expr, AST_expr>())
				| (REAL_LIT ^ make_as<AST_real_lit_expr, AST_expr>())
				| (TRUE ^ make_as<AST_bool_lit_expr, AST_expr>())
				| (FALSE ^ make_as<AST_bool_lit_expr, AST_expr>())
				| (IDENT ^ make_as<AST_ident_expr, AST_expr>())
				| (IfExpr ^ cast<AST_expr>())
				| LetExpr
				| (BlockExpr ^ cast<AST_expr>())
				| ((LPAREN >= RPAREN) ^ make_as<AST_list_expr, AST_expr>())
				| (LPAREN > ExprList < !RPAREN)
				;

			const auto lvl1 =
				(lvl0 >= *(LPAREN > &CallParamList >= !RPAREN))
				^ [](auto& fn, auto& params) -> AST_expr* {
					return fnl::fold(params.begin(), params.end(), fn,
						[](auto& fn, auto& param_ls) -> AST_expr* {
							auto& param_pack = std::get<0>(param_ls);
							auto& rparen = std::get<1>(param_ls);
							if (param_pack) {
								auto& fpar = std::get<0>(*param_pack);
								auto& restpars = std::get<1>(*param_pack);
								restpars.insert(restpars.begin(), fpar);
								return new AST_call_expr(fn, restpars, rparen);
							}
							else {
								return new AST_call_expr(fn, yvec<AST_expr*>{}, rparen);
							}
						});
				};

			const auto lvl2 =
				(*(PLUS | MINUS) >= lvl1)
				^ [](auto& ops, auto& exp) {
					return fnl::fold(ops.begin(), ops.end(), exp,
						[](auto& exp, auto& op) -> AST_expr* {
							return new AST_pre_expr(op, exp);
						});
				};

			const auto lvl3 =
				(lvl2 >= *((STAR | SLASH | PERCENT) >= !(lvl2 / "expression")))
				^ foldl_bintree<AST_bin_expr>();

			const auto lvl4 =
				(lvl3 >= *((PLUS | MINUS) >= !(lvl3 / "expression")))
				^ foldl_bintree<AST_bin_expr>();

			const auto lvl5 =
				(lvl4 >= *((GREATER | LESS | GREQ | LEEQ) >= !(lvl4 / "expression")))
				^ foldl_bintree<AST_bin_expr>();

			const auto lvl6 =
				(lvl5 >= *((EQ | NEQ) >= !(lvl5 / "expression")))
				^ foldl_bintree<AST_bin_expr>();

			const auto lvl7 =
				(lvl6 >= *(ASGN >= !(lvl6 / "expression")))
				^ foldr_bintree<AST_bin_expr>();

			const auto lvl_list =
				(Expr >= *(COMMA > !Expr)) ^ list_or_single<AST_list_expr, AST_expr>();

			result_t<AST_expr*> parse_expr(token_input& in) {
				return lvl7(in);
			}

			result_t<AST_expr*> parse_expr_list(token_input& in) {
				return lvl_list(in);
			}
		}

		static auto Expr = expr_detail::Expr;
		static auto ExprList = expr_detail::ExprList;
		static auto FnExpr = expr_detail::FnExpr;
		static auto IfExpr = expr_detail::IfExpr;
		static auto BlockExpr = expr_detail::BlockExpr;

		/*********************************************************************/

		namespace stmt_detail {
			auto scope_as_stmt() {
				return [](auto& e) {
					e->AsStatement = true;
					return e;
				};
			}

			const auto parse_block_stmt_impl =
				BlockExpr ^ scope_as_stmt();

			const auto IfStmt =
				(IF >= !(ExprList / "condition") >= !BlockStmt
					>= *(ELSE > IF >= !(ExprList / "condition") >= !BlockStmt)
					>= &(ELSE > !BlockStmt))
				^ [](auto& beg, auto& cond, auto& then, auto& elifs, auto& el) -> AST_stmt* {
					yopt<AST_block_expr*> elbody =
						fnl::fold(elifs.rbegin(), elifs.rend(), el,
							[=](auto& curr, auto& elem) -> AST_block_expr* {
								auto res = new AST_if_expr(
									std::get<0>(elem),
									std::get<1>(elem),
									std::get<2>(elem),
									curr);
								res->AsStatement = true;
								return new AST_block_expr(new AST_expr_stmt(res));
							});
					auto res = new AST_if_expr(beg, cond, then, elbody);
					res->AsStatement = true;
					return new AST_expr_stmt(res);
				};

			const auto Stmt =
				  ((DBG_WR_T >= ExprList) ^ make_as<AST_dbg_write_ty_stmt, AST_stmt>())
				| Decl
				| IfStmt
				| (BlockStmt ^ make_as<AST_expr_stmt, AST_stmt>())
				| ((ExprList >= SEMICOL) ^ make_as<AST_expr_stmt, AST_stmt>())
				;

			const auto Decl =
				  ((FN >= !IDENT < !ASGN >= !(FnExpr / "function expression"))
				  ^ make_as<AST_fn_decl_stmt, AST_stmt>())
				| ((TYPE >= !IDENT < !ASGN >= !(TypeList / "type"))
				  ^ make_as<AST_ty_decl_stmt, AST_stmt>())
				;
		}

		result_t<AST_block_expr*> parse_block_stmt(token_input& in) {
			return stmt_detail::parse_block_stmt_impl(in);
		}

		result_t<AST_stmt*> parse_stmt(token_input& in) {
			return stmt_detail::Stmt(in);
		}

		result_t<AST_stmt*> parse_decl(token_input& in) {
			return stmt_detail::Decl(in);
		}

		const auto Program =
			*Decl < !END_OF_F;
	}

	result_t<yvec<AST_stmt*>> parse_program(token_input& in) {
		return Program(in);
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
