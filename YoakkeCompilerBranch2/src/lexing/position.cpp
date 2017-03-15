#include "position.h"

namespace yk {
	position::position(ysize r, ysize c)
		: Row(r), Col(c) {
	}

	interval::interval(position const& st, ysize len)
		: Start(st.Row, st.Col - len), End(st) {
	}
}
