/**
 * code_formatter.h
 * Created by: Peter Lenkefi
 * All rights reserved.
 *
 * Description:
 *  Here is all the functionality for outputting formatted and annotated pieces
 *  of code.
 */

#pragma once

#include <iostream>
#include "common.h"
#include "file_hnd.h"
#include "position.h"

struct code_formatter {
public:
	static std::ostream* Out;	// Output stream
	static ysize BufferW;		// Width of the output buffer
	static ysize LinesBefore;	// How many lines to print before position
	static ysize LinesAfter;	// How many lines to print after position
	static ysize MaxBetween;	// How many lines to print at most
	static ysize TabSize;		// Column alignment size

public:
	/**
	 * Prints a piece from a file at the given position, underlining
	 * the position.
	 * @param file The source file.
	 * @param pos The position to annotate.
	 */
	static void print(file_hnd const& file, interval pos, yopt<interval> pos2 = {});

private:
	static ystr s_LineSep; // Separates line numbering from content

private:
	/**
	 * Calculates the first and the last line indicies that should be printed
	 * based on the file and the annotated position.
	 * @param file The souce file.
	 * @param from The first annotated line.
	 * @param to The last annotated line.
	 * @return A pair of indicies in the form of (from, to).
	 */
	static ypair<ysize, ysize> get_bounds(file_hnd const& file, ysize from, ysize to);

	static void print_line(file_hnd const& file, ysize idx, ysize max_digs);

	static void print_line_begin(bool first, ysize idx, ysize max_digs);

	static ysize print_line_sect(ysize idx, const char* src, ysize offs, ysize line_len, ysize max_digs);

public:
	code_formatter() = delete; // Cannot instantiate
};
