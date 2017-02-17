#pragma once

#include "../common.h"

namespace yk {
	class token {
	private:
		ystr m_Identifier;
		ystr m_Value;

	public:
		token(ystr const& id, ystr const& val);

	public:
		ystr const& value() const;
		ystr const& identifier() const;
	};
}