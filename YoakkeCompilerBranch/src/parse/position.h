#pragma once

#include "token.h"
#include "../common.h"

namespace yk {
	struct point {
	public:
		ysize StartX;
		ysize StartY;
		ysize EndX;
		ysize EndY;

	public:
		point(ysize sx, ysize sy, ysize ex, ysize ey);

	public:
		static point get(token const& tok);
		static point interval(point const& a, point const& b);
	};
}