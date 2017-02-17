#pragma once

#include "token.h"
#include "../common.h"

namespace yk {
	struct position {
	public:
		ysize StartX;
		ysize StartY;
		ysize EndX;
		ysize EndY;

	public:
		position(ysize sx, ysize sy, ysize ex, ysize ey);

	public:
		static position get(token const& tok);
		static position interval(position const& a, position const& b);
	};
}