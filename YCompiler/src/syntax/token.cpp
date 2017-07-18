#include "token.h"

token::token(interval const& pos, token_t type, ystr const& val)
	: Pos(pos), Type(type), Value(val) {
}

token::token()
	: token(interval(), token_t::Epsilon) {
}

ystr token::fmt() const {
	switch (Type) {
	case token_t::Epsilon:
		return "<epsilon>";

	case token_t::EndOfFile:
		return "end-of-file";

	case token_t::Ident:
		return "identifier";

	case token_t::IntLit:
		return "integer literal";

	case token_t::RealLit:
		return "real literal";

	default:
		return '\'' + Value + '\'';
	}
	return "<error>";
}
