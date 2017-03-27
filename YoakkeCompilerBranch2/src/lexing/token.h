#pragma once

#include "position.h"
#include "../common.h"

namespace yk {
	class token {
	public:
		using type_t = ysize;

	public:
		type_t Type;
		ystr Value;
		interval Position;

	public:
		token()
			: Type(0), Value(""), Position(position(0, 0), 0) {
		}

		template <typename T>
		token(T tt, ystr const& val, position const& pos)
			: Type(static_cast<type_t>(tt)), Value(val),
			Position(pos, val.length()) {
		}
	};
}
