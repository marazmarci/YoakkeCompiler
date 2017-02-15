#include "token.h"

namespace yk {
	token::token(ystr const& id, ystr const& val)
		: m_Identifier(id), m_Value(val) {
	}

	ystr const& token::value() {
		return m_Value;
	}

	ystr const& token::identifier() {
		return m_Identifier;
	}
}