/**
 * position.h
 * Created by: Peter Lenkefi
 * All rights reserved.
 *
 * Description:
 *  These are descriptors of places in the file, mainly used for error
 *  reporting and error IO.
 */

#pragma once

#include "common.h"

/**
 * Describes a single place in the file, like a character.
 */
struct point {
	ysize Column;	// x position
	ysize Row;		// y position

	/**
	 * Create a point at the given coordinates.
	 * @param c The column (x).
	 * @param r The row (y).
	 */
	point(ysize c, ysize r)
		: Column(c), Row(r) {
	}
};

/**
 * Describes an interval in file from start to finish.
 */
struct interval {
	point Start;	// Beginning point
	point End;		// Ending point

	/**
	 * Creates an interval from two points.
	 * @param s The starting point.
	 * @param e The ending points.
	 */
	interval(point const& s, point const& e)
		: Start(s), End(e) {
	}

	/**
	 * Creates an interval from an ending point and a length.
	 * @param ep The ending point.
	 * @param len The length of the interval.
	 */
	interval(point const& ep, ysize len)
		: Start(point(ep.Column - len, ep.Row)), End(ep) {
	}

	/**
	 * Creates an interval from two other intervals.
	 * @param i1 The first interval.
	 * @param i2 The second interval.
	 */
	interval(interval const& i1, interval const& i2)
		: Start(i1.Start), End(i2.End) {
	}
};
