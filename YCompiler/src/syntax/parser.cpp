#include "ast.h"
#include "parser.h"
#include "token.h"

namespace parser {
	result_t<token> p_eof(token_input& in) {
		return terminal<token_t::EndOfFile>("end-of-file")(in);
	}

	result_t<label> p_label(token_input& in) {
		auto LBracket = terminal<token_t::LBracket>("'['");
		auto RBracket = terminal<token_t::RBracket>("']'");
		auto Ident = terminal<token_t::Ident>("identifier");

		auto label_parser = LBracket >= Ident >= RBracket;
		auto res = label_parser(in);
		if (res.is_ok()) {
			auto& res_ok = res.get_ok();
			auto& toks = std::get<0>(res_ok);
			auto& in2 = std::get<1>(res_ok);
			
			auto& left	= std::get<0>(toks);
			auto& name	= std::get<1>(toks);
			auto& right = std::get<2>(toks);

			return std::make_tuple(label(interval(left.Pos, right.Pos), name), in2);
		}
		else {
			return res.get_err();
		}
	}
}
