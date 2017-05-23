#include "token.h"

namespace yk {
	token::token()
		: Type(ytoken_t(0)), Value(""), Position(position(0, 0), 0) {
	}

	token::token(ytoken_t tt, ystr const& val, position const& pos)
		: Type(tt), Value(val),
		Position(pos, val.length()) {
	}

	ystr token::format() const {
		switch (Type) {
		case ytoken_t::Ident:
			return "identifier";

		case ytoken_t::Integer:
			return "integer literal";

		case ytoken_t::Real:
			return "real literal";

		case ytoken_t::EndOfFile:
			return "end of file";

		case ytoken_t::Epsilon:
			return "<unknown>";

		default:
			return '\'' + Value + '\'';
		}
	}
}
