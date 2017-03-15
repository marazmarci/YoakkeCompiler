#pragma once

#include "position.h"
#include "../common.h"

namespace yk {
	class token {
	public:
		ysize Type;
		ystr Value;
		interval Position;

	public:
		template <typename T>
		token(T tt, ystr const& val, position const& pos)
			: Type(static_cast<ysize>(tt)), Value(val), 
			Position(pos, val.length()) {
		}
	};
}
