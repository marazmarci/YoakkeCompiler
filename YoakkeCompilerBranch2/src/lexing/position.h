#pragma once

#include "../common.h"

namespace yk {
	class position {
	public:
		ysize Row;
		ysize Col;

	public:
		position(ysize r, ysize c);
	};

	class interval {
	public:
		position Start;
		position End;

	public:
		interval();
		interval(position const& st, ysize len);
		interval(interval const& a, interval const& b);
	};
}
