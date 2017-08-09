#include <iostream>
#include "ast.h"
#include "ast_ty.h"
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

		const parser_t<AST_ty*> Type = parse_type;
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
			(LParen > &(Param >= *(Comma > !Param)) < !RParen)
			^ [](auto& res) { 
				if (res) {
					auto& res_val = res.value();
					auto param0 = make_result_list(
						std::get<0>(res_val), 
						std::get<1>(res_val));
					auto& vec = std::get<2>(res_val);
					vec.insert(vec.begin(), param0);
					return vec;
				}
				else {
					return yvec<result_list<yopt<token>, AST_ty*>>{};
				}
			};
		using RES_T = std::result_of_t<decltype(ParamList)(token_input&)>;
		static auto FnParser =
			Fn >= &Ident >= &(LParen >= &(Param >= *(Comma >= !Param)) >= !RParen);

		std::cout << typeid(RES_T).name() << std::endl;

		//return FnParser(in);
		return std::make_tuple(((AST_expr*)nullptr), in);
	}
}
