#include "token.h"

namespace yk {
	token::token(ystr const& id, ystr const& val)
		: m_Identifier(id), m_Value(val) {
	}

	token::~token() { }

	ystr const& token::value() const {
		return m_Value;
	}

	ystr const& token::identifier() const {
		return m_Identifier;
	}
}