#include "position.h"
#include "token.h"

namespace yk {
	position::position(ysize sx, ysize sy, ysize ex, ysize ey)
		: StartX(sx), StartY(sy), EndX(ex), EndY(ey) {
	}

	// TODO: add token positioning
	position position::get(token const& tok) {
		return position(0, 0, 0, 0);
	}
}