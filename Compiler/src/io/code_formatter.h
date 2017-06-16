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

#include <cassert>
#include <iostream>
#include "file_hnd.h"
#include "../common.h"
#include "../syntax/position.h"

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
	static ystr s_LineSep;		// Separates line numbering from content
	static ystr s_LineSepInt;	// Separator in intervals

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

	/**
	 * Prints a line numbering or the space for the numbering followed by the
	 * separator pipes.
	 * @param first The line number is printed if true, spaces otherwise.
	 * @param idx The line number.
	 * @param max_digs The digit count of the highest printed line number.
	 */
	static void print_line_begin(bool first, ysize idx, ysize max_digs);
	
	/**
	 * Prints a line numbering or the space for the numbering followed by the
	 * separator pipes when inside an interval notation.
	 * @param first The line number is printed if true, spaces otherwise.
	 * @param idx The line number.
	 * @param max_digs The digit count of the highest printed line number.
	 */
	static void print_line_begin_in(bool first, ysize idx, ysize max_digs);

	/**
	 * Expands a line, removing all special characters, like tabs.
	 * @param line The pointer to the beinning of the line.
	 * @param len The length of the line.
	 * @return The expanded line string.
	 */
	static ystr expand_line(const char* line, ysize len);

	/**
	 * Expands a line, removing all special characters, like tabs and tracks
	 * given points and their changes in space.
	 * @param line The pointer to the beinning of the line.
	 * @param len The length of the line.
	 * @param points The points that are tracked and modified to their new
	 * place accordingly.
	 * @return The expanded line string.
	 */
	static ystr expand_line(const char* line, ysize len, yvec<ypair<ysize, ysize>*>& points);
	
	/**
	 * A helper function for expand_line..
	 * @see expand_line
	 */
	static void expand_until(const char* line, ysize& start, ysize end, ystr& result);
	
	/**
	 * Generates arrows pointing to the given intervals.
	 * @param beg The beginning position of the arrow.
	 * @param points The pointed intervals.
	 * @return A string containing the arrow.
	 */
	static ystr generate_arrows(ysize beg, yvec<ypair<ysize, ysize>> const& points);

	// Various print functions that have a lot of common structures
	// TODO: refactor these, lot of common code
	static void print_line(file_hnd const& file, ysize idx, ysize max_digs);
	static void print_line(file_hnd const& file, ysize idx, ysize max_digs, yvec<ypair<ysize, ysize>> points);
	static void print_line_beg(file_hnd const& file, ysize idx, ysize max_digs, yvec<ypair<ysize, ysize>> points);
	static void print_line_in(file_hnd const& file, ysize idx, ysize max_digs);
	static void print_line_end(file_hnd const& file, ysize idx, ysize max_digs, yvec<ypair<ysize, ysize>> points);

public:
	code_formatter() = delete; // Cannot instantiate
};
