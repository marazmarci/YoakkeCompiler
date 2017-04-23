#include "token.h"

namespace yk {
	token::token()
		: Type(ytoken_t(0)), Value(""), Position(position(0, 0), 0) {
	}

	token::token(ytoken_t tt, ystr const& val, position const& pos)
		: Type(tt), Value(val),
		Position(pos, val.length()) {
	}
}
