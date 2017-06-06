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
	static void print(file_hnd const& file, interval const& pos);

private:
	static ystr s_LineSep; // Separates line numbering from content

private:
	static void print_line(file_hnd const& file, ysize idx, ysize digit_cnt);
	static void print_line_annot(file_hnd const& file, ysize idx, ysize digit_cnt, ysize start, ysize end);
	static ysize print_line_segment(const char* src, ysize idx, ysize offs, ysize len, ysize digit_cnt, yvec<yref<ysize>>& points = yvec<yref<ysize>>{});
	static void print_line_number(bool first, ysize num, ysize digit_cnt);

public:
	code_formatter() = delete; // Cannot instantiate
};
