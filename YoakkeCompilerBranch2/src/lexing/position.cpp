#include "position.h"

namespace yk {
	position::position(ysize r, ysize c)
		: Row(r), Col(c) {
	}

	interval::interval(position const& st, ysize len)
		: Start(st.Row, st.Col - len), End(st) {
	}

	interval::interval(interval const& a, interval const& b)
		: Start(a.Start), End(b.End) {
	}
}
