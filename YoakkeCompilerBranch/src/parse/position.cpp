#include "position.h"

namespace yk {
	point::point(ysize sx, ysize sy, ysize ex, ysize ey)
		: StartX(sx), StartY(sy), EndX(ex), EndY(ey) {
	}

	// TODO: add token positioning
	point point::get(token const& tok) {
		return point(0, 0, 0, 0);
	}

	point point::interval(point const& a, point const& b) {
		return point(a.StartX, a.StartY, a.EndX, a.EndY);
	}
}