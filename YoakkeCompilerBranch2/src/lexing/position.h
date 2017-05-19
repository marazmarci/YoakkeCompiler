#pragma once

#include "../common.h"

namespace yk {
	/*
	A single x, y position in files.
	*/
	class position {
	public:
		ysize Row;
		ysize Col;

	public:
		position(ysize r, ysize c);
	};

	/*
	Represents an interval (x1, y1) to (x2, y2) in a file.
	*/
	class interval {
	public:
		position Start;
		position End;

	public:
		interval();
		interval(position const& st, ysize len);
		interval(interval const& a, interval const& b);
	};

	interval operator*(interval const& a, interval const& b);
}
