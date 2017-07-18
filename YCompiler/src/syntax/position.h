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

#include "../common.h"

/**
 * Describes a single place in the file, like a character.
 */
struct point {
	ysize Column;	// x position
	ysize Row;		// y position

	/**
	 * Creates a point at the given coordinates.
	 * @param c The column (x).
	 * @param r The row (y).
	 */
	point(ysize c, ysize r);

	/**
	 * Creates a point at 0, 0.
	 */
	point();
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
	interval(point const& s, point const& e);

	/**
	 * Creates an interval from an ending point and a length.
	 * @param ep The ending point.
	 * @param len The length of the interval.
	 */
	interval(point const& ep, ysize len);

	/**
	 * Creates an interval from two other intervals.
	 * @param i1 The first interval.
	 * @param i2 The second interval.
	 */
	interval(interval const& i1, interval const& i2);

	/**
	 * Creates an empty interval at 0, 0.
	 */
	interval();
};
