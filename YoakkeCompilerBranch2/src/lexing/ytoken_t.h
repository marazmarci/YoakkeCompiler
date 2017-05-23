#pragma once

namespace yk {
	enum class ytoken_t {
		EndOfFile,
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
		Ignore,

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

		Let,
		Typeof
	};
}
