#pragma once

/**
 * fmt_code.h
 * Created by: Peter Lenkefi
 * All rights reserved.
 *
 * Description:
 *  Here is all the functionality for outputting formatted and annotated pieces
 *  of code.
 */

#pragma once

#include "file_hnd.h"
#include "../common.h"
#include "../syntax/position.h"

namespace fmt_code {
	// Settings

	extern ysize BufferWidth;
	extern ysize LinesBefore;
	extern ysize LinesAfter;
	extern ysize MaxBetween;
	extern ysize TabSize;

	// Functions

	/**
	 * Prints a piece from a file at the given position, underlining
	 * the position.
	 * @param file The source file.
	 * @param pos The position to annotate.
	 */
	void print(file_hnd const& file, interval const& pos);

	/**
	 * Prints a piece from a file at the given position, underlining
	 * the interval.
	 * @param file The source file.
	 * @param pos1 The beginning position to annotate.
	 * @param pos2 The ending position to annotate.
	 */
	void print(file_hnd const& file, interval const& pos1, interval const& pos2);
}

