#include "position.h"

namespace yk {
	position::position(ysize sx, ysize sy, ysize ex, ysize ey)
		: StartX(sx), StartY(sy), EndX(ex), EndY(ey) {
	}

	// TODO: add token positioning
	position position::get(token const& tok) {
		return position(0, 0, 0, 0);
	}

	position position::interval(position const& a, position const& b) {
		return position(a.StartX, a.StartY, a.EndX, a.EndY);
	}
}