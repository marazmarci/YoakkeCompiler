#pragma once

#include "../lexing/token.h"

namespace yk {
	enum class ytoken_t {
		Epsilon,
		Ident,
		Integer,
		Real,
		Lpar,
		Rpar,
		Lsqbr,
		Rsqbr,
		Lbrace,
		Rbrace,
		Colon,
		Dcolon,
		Arrow,
		Semicol,
	};
}
