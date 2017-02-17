#pragma once

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
	};
}