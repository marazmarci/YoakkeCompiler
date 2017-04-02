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
		Arrow,
		Semicol,

		Dcolon,
		Comma,
		Asgn,
		Or,
		And,
		Eq, Neq,
		Gr, Le, Goe, Loe,
		Add, Sub,
		Mul, Div, Mod,
		Dot,
		Not,

		Let
	};
}
