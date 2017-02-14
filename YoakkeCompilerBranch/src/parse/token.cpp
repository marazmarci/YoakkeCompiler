#include "token.h"

namespace yk {
	keyword_token::keyword_token(ystr const& val)
		: Value(val) { }

	literal_token::literal_token(ystr const& id, ystr const& val)
		: Identifier(id), Value(val) { }

	token::token(ystr const& val) {
		m_Data = keyword_token(val);
	}

	token::token(ystr const& id, ystr const& val) {
		m_Data = literal_token(id, val);
	}

	bool token::is_keyword() {
		return m_Data.is<keyword_token>();
	}

	bool token::is_literal() {
		return m_Data.is<literal_token>();
	}

	ystr token::value() {
		if (is_keyword()) {
			return m_Data.get<keyword_token>().Value;
		}
		else {
			return m_Data.get<literal_token>().Value;
		}
	}

	ystr token::identifier() {
		return m_Data.get<literal_token>().Identifier;
	}
}