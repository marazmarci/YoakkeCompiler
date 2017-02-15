#pragma once

#include "../common.h"

namespace yk {
	class token {
	private:
		const ystr m_Identifier;
		const ystr m_Value;

	public:
		token(ystr const& id, ystr const& val);

	public:
		ystr const& value();
		ystr const& identifier();
	};
}