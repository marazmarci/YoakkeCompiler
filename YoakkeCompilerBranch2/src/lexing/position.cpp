#include "position.h"

namespace yk {
	point::point(ysize r, ysize c)
		: Row(r), Col(c) {
	}

	interval::interval()
		: Start(0, 0), End(0, 0) {
	}

	interval::interval(point const& st, ysize len)
		: Start(st.Row, st.Col - len), End(st) {
	}

	interval::interval(interval const& a, interval const& b)
		: Start(a.Start), End(b.End) {
	}

	interval operator*(interval const& a, interval const& b) {
		return interval(a, b);
	}
}
