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
struct position {
	ysize Column;	// x position
	ysize Row;		// y position

	/**
	 * Create a position at the given coordinates.
	 * @param c The column (x).
	 * @param r The row (y).
	 */
	position(ysize c, ysize r)
		: Column(c), Row(r) {
	}
};

/**
 * Describes an interval in file from start to finish.
 */
struct interval {
	position Start;	// Beginning position
	position End;	// Ending position

	/**
	 * Creates an interval from two positions.
	 * @param s The starting position.
	 * @param e The ending position.
	 */
	interval(position const& s, position const& e)
		: Start(s), End(e) {
	}

	/**
	 * Creates an interval from an ending position and a length.
	 * @param ep The ending position.
	 * @param len The length of the interval.
	 */
	interval(position const& ep, ysize len)
		: Start(position(ep.Column - len, ep.Row)), End(ep) {
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
