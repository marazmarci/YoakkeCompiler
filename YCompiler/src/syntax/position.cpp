#include "position.h"

// point

point::point(ysize c, ysize r)
	: Column(c), Row(r) {
}

point::point()
	: point(0, 0) {
}

// interval

interval::interval(point const& s, point const& e)
	: Start(s), End(e) {
}

interval::interval(point const& ep, ysize len)
	: Start(point(ep.Column - len, ep.Row)), End(ep) {
}

interval::interval(interval const& i1, interval const& i2)
	: Start(i1.Start), End(i2.End) {
}

interval::interval()
	: interval(point(), point()) {
}
