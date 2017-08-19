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
		const auto Ident	= terminal<token_t::Ident>("identifier");
		const auto LParen	= terminal<token_t::LParen>("'('");
		const auto RParen	= terminal<token_t::RParen>("')'");
		const auto LBrace	= terminal<token_t::LBrace>("'{'");
		const auto RBrace	= terminal<token_t::RBrace>("'}'");
		const auto Colon	= terminal<token_t::Colon>("':'");
		const auto Comma	= terminal<token_t::Comma>("','");
		const auto Arrow	= terminal<token_t::Arrow>("'->'");
		const auto Plus		= terminal<token_t::Add>("'+'");
		const auto Minus	= terminal<token_t::Sub>("'-'");
		const auto Star		= terminal<token_t::Mul>("'*'");
		const auto Slash	= terminal<token_t::Div>("'/'");
		const auto Percent	= terminal<token_t::Mod>("'%'");
		const auto Greater	= terminal<token_t::Greater>("'>'");
		const auto Less		= terminal<token_t::Less>("'<'");
		const auto Goe		= terminal<token_t::GrEq>("'>='");
		const auto Loe		= terminal<token_t::LeEq>("'<='");
		const auto Equals	= terminal<token_t::Eq>("'=='");
		const auto NEquals	= terminal<token_t::Neq>("'<>'");
		const auto Fn		= terminal<token_t::Fn>("'fn'");
		const auto Asgn		= terminal<token_t::Asgn>("'='");
		const auto Let		= terminal<token_t::Let>("'let'");
		const auto Semicol	= terminal<token_t::Semicol>("';'");
		const auto If		= terminal<token_t::If>("'if'");
		const auto Else		= terminal<token_t::Else>("'else'");
		const auto IntLit	= terminal<token_t::IntLit>("integer literal");
		const auto RealLit	= terminal<token_t::RealLit>("real literal");
		const auto Eof		= terminal<token_t::EndOfFile>("end-of-file");
		const auto DbgWrTy	= terminal<token_t::DbgWriteTy>("<debug>");

		const parser_t<AST_ty*>		Type	= parse_type;
		const parser_t<AST_stmt*>	Stmt	= parse_stmt;
		const parser_t<AST_expr*>	Expr	= parse_expr;
		const parser_t<AST_pat*>	Pattern = parse_pat;

		const parser_t<AST_block_expr*> Block	 = parse_block_expr;
		const parser_t<AST_decl_stmt*>	Decl	 = parse_decl_stmt;
		const parser_t<AST_fn_expr*>	FnExpr	 = parse_fn_expr;
		const parser_t<AST_let_expr*>	LetExpr  = parse_let_expr;
		const parser_t<AST_if_expr*>	IfExpr	 = parse_if_expr;
		const parser_t<AST_expr*>		ListExpr = parse_list_expr;
	}

	result_t<yvec<AST_stmt*>> parse_program(token_input& in) {
		static auto ProgComponent =
			  (Decl ^ [](auto& d) -> AST_stmt* { return d; })
			| ((DbgWrTy >= ListExpr) ^ [](auto& beg, auto& exp)->AST_stmt*  {
				return new AST_dbg_write_ty_stmt(beg, exp);
			  })
			;
		static auto program_parser =
			*ProgComponent < !Eof;

		return program_parser(in);
	}

	result_t<AST_ty*> parse_type(token_input& in) {
		static auto lvl0 =
			  (Ident ^ [](auto& tok) -> AST_ty* { return new AST_ident_ty(tok); })
			| ((LParen >= RParen) ^ [](auto& lp, auto& rp) -> AST_ty* {
				return new AST_list_ty(lp, rp);
			  })
			| ((LParen > Type < !RParen) ^ [](auto& t) { return t; });

		static auto lvl1 =
			((lvl0 >= *(Arrow >= !lvl0))
			^ [](auto& left, auto& rights) -> AST_ty* {
				if (rights.size()) {
					auto it = rights.rbegin();
					auto op = std::get<0>(*it);
					auto curr = std::get<1>(*it);
					it++;
					while (it != rights.rend()) {
						auto& tup = *it;
						auto right = std::get<1>(tup);
						curr = new AST_bin_ty(right, op, curr);
						op = std::get<0>(tup);
						it++;
					}
					return new AST_bin_ty(left, op, curr);
				}
				else {
					return left;
				}
			});

		static auto lvl2 =
			(((lvl1 % "list_ty/first_ty") >= *((Comma > !(lvl1)) % "list_ty/next_ty"))
			^ [](auto& first, auto& rest) -> AST_ty* {
				if (rest.size()) {
					rest.insert(rest.begin(), first);
					return new AST_list_ty(rest);
				}
				else {
					return first;
				}
			});

		return lvl2(in);
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
			(LBrace >= (*(Stmt) % "block/stmt") >= &(ListExpr % "block/return") >= !RBrace)
			^ [](auto& lbr, auto& stmt_list, auto& val, auto& rbr) -> AST_block_expr* {
				return new AST_block_expr(lbr, stmt_list, val, rbr);
			};

		return block_parser(in);
	}

	result_t<AST_stmt*> parse_stmt(token_input& in) {
		static auto stmt_parser =
				((DbgWrTy >= ListExpr) ^ [](auto& beg, auto& exp)->AST_stmt* {
					return new AST_dbg_write_ty_stmt(beg, exp);
				})
			| (Decl ^ [](auto& res) -> AST_stmt* { return res; })
			| (IfExpr ^ [](auto& e) -> AST_stmt* { e->AsStatement = true; return new AST_expr_stmt(e); })
			| (Block ^ [](auto& e) -> AST_stmt* { e->AsStatement = true; return new AST_expr_stmt(e); })
			| ((((ListExpr) >= Semicol) ^ [](auto& ex, auto& sc) -> AST_stmt* { return new AST_expr_stmt(ex, sc); }) % "stmt/list_expr");

		return stmt_parser(in);
	}

	result_t<AST_expr*> parse_expr(token_input& in) {
		static auto param_list =
			Expr >= *(Comma > !Expr);
		
		static auto LeftAssocTree = [](auto& left, auto& rights) -> AST_expr* {
			return fnl::fold(rights.begin(), rights.end(), left,
				[](auto& left, auto& rhs) -> AST_expr* {
				auto& op = std::get<0>(rhs);
				auto& right = std::get<1>(rhs);
				return new AST_bin_expr(left, op, right);
			});
		};

		static auto RightAssocTree = [](auto& left, auto& rights) -> AST_expr* {
			if (rights.size()) {
				auto it = rights.rbegin();
				auto op = std::get<0>(*it);
				auto curr = std::get<1>(*it);
				it++;
				while (it != rights.rend()) {
					auto& tup = *it;
					auto right = std::get<1>(tup);
					curr = new AST_bin_expr(right, op, curr);
					op = std::get<0>(tup);
					it++;
				}
				return new AST_bin_expr(left, op, curr);
			}
			else {
				return left;
			}
		};

		static auto lvl0 =
			  (IntLit ^ [](auto& i) -> AST_expr* { return new AST_int_lit_expr(i); })
			| (RealLit ^ [](auto& i) -> AST_expr* { return new AST_real_lit_expr(i); })
			| ((Ident ^ [](auto& in) -> AST_expr* { return new AST_ident_expr(in); }) % "expr/lvl0/rule0")
			| ((IfExpr ^ [](auto& exp) -> AST_expr* { return exp; }) % "expr/lvl0/rule1")
			| ((LetExpr ^ [](auto& exp) -> AST_expr* { return exp; }) % "expr/lvl0/rule2")
			| ((Block ^ [](auto& exp) -> AST_expr* { return exp; }) % "expr/lvl0/rule6")
			| (((LParen >= RParen)
				^ [](auto& lp, auto& rp) -> AST_expr* {
					return new AST_list_expr(lp, rp);
				}) % "expr/lvl0/rule3")
			| (((LParen > ListExpr < !RParen) 
				^ [](auto& ls) -> AST_expr* { 
					return ls;
				}) % "expr/lvl0/rule4")
			;

		static auto lvl1 =
			((lvl0 >= *(LParen > &param_list >= !RParen))
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
			}) % "expr/lvl1";

		static auto lvl2 =
			((*(Plus | Minus) >= lvl1) ^ [](auto& ops, auto& exp) -> AST_expr* {
				return fnl::fold(ops.begin(), ops.end(), exp,
				[](auto& exp, auto& op) -> AST_expr* {
					return new AST_pre_expr(op, exp);
				});
			}) % "expr/lvl2";

		static auto lvl3 =
			((lvl2 >= *((Star | Slash | Percent) >= !(lvl2 / "expression")))
			^ LeftAssocTree) % "expr/lvl3";

		static auto lvl4 =
			((lvl3 >= *((Plus | Minus) >= !(lvl3 / "expression")))
			^ LeftAssocTree) % "expr/lvl4";

		static auto lvl5 =
			((lvl4 >= *((Greater | Less | Goe | Loe) >= !(lvl4 / "expression")))
			^ LeftAssocTree) % "expr/lvl5";

		static auto lvl6 =
			((lvl5 >= *((Equals | NEquals) >= !(lvl5 / "expression")))
			^ LeftAssocTree) % "expr/lvl6";

		static auto lvl7 =
			((lvl6 >= *((Asgn) >= !(lvl6 / "expression")))
			^ RightAssocTree) % "expr/lvl7";

		return lvl7(in);
	}

	result_t<AST_pat*> parse_pat(token_input& in) {
		static auto lvl0 =
			  (Ident ^ [](auto& tok) -> AST_pat* { return new AST_ident_pat(tok); })
			| ((LParen >= RParen) ^ [](auto& lp, auto& rp) -> AST_pat* {
				return new AST_list_pat(lp, rp);
			  })
			| ((LParen > Pattern < !RParen) ^ [](auto& t) { return t; });

		static auto lvl1 =
			(((lvl0 % "list_pat/first_pat") >= *((Comma > !(lvl0)) % "list_pat/next_pat"))
			^ [](auto& first, auto& rest) -> AST_pat* {
				if (rest.size()) {
					rest.insert(rest.begin(), first);
					return new AST_list_pat(rest);
				}
				else {
					return first;
				}
			});

		return lvl1(in);
	}

	result_t<AST_decl_stmt*> parse_decl_stmt(token_input& in) {
		static auto decl_parser =
			(Fn >= !Ident < !Asgn >= !(FnExpr / "function expression"))
			^ [](auto& beg, auto& id, auto& fn) -> AST_decl_stmt* {
				return new AST_decl_stmt(beg, id, fn);
			};

		return decl_parser(in);
	}

	result_t<AST_let_expr*> parse_let_expr(token_input& in) {
		static auto let_parser =
			(Let >= !(Pattern / "pattern")
			>= &(Colon > !(Type / "type"))
			>= &(Asgn > !(ListExpr / "expression")))
			^ [](auto& beg, auto& pat, auto& ty, auto& val) -> AST_let_expr* {
				return new AST_let_expr(beg, pat, ty, val);
			};

		return let_parser(in);
	}

	result_t<AST_if_expr*> parse_if_expr(token_input& in) {
		static auto if_parser =
			(If >= !(Expr / "condition") >= !(Block)
			>= *(Else > If >= !(Expr / "condition") >= !(Block))
			>= &(Else > !Block))
			^ [](auto& beg, auto& cond, auto& then, auto& elifs, auto& el) -> AST_if_expr* {
				yopt<AST_block_expr*> elbody = 
					fnl::fold(elifs.rbegin(), elifs.rend(), el,
					[](auto& curr, auto& elem) -> AST_block_expr* {
						auto& e_tok = std::get<0>(elem);
						auto& e_cond = std::get<1>(elem);
						auto& e_block = std::get<2>(elem);
						AST_if_expr* eif = new AST_if_expr(e_tok, e_cond, e_block, curr);
						return new AST_block_expr(new AST_expr_stmt(eif));
					});
				return new AST_if_expr(beg, cond, then, elbody);
			};

		return if_parser(in);
	}

	result_t<AST_expr*> parse_list_expr(token_input& in) {
		static auto list_parser =
			(((Expr % "list_expr/first_expr") >= *((Comma > !(Expr)) % "list_expr/next_expr"))
			^ [](auto& first, auto& rest) -> AST_expr* {
				if (rest.size()) {
					rest.insert(rest.begin(), first);
					return new AST_list_expr(rest);
				}
				else {
					return first;
				}
			});

		return list_parser(in);
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
