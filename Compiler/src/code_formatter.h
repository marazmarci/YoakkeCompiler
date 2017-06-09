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

	static void print_line_begin(bool first, ysize idx, ysize max_digs);
	static ystr expand_line(const char* line, ysize len, std::initializer_list<yref<ysize>> points = {});

private:
	template <typename... Ts>
	static void print_line(file_hnd const& file, ysize idx, ysize max_digs, Ts... points) {
		// Create an initializer list from the points so we can iterate over them
		std::initializer_list<ysize> points_list = { points... };
		
		// Get a reference to the output stream for simpler syntax
		std::ostream& outs = *Out;

		// The actual text buffer size
		assert(BufferW > max_digs + s_LineSep.length() && "The buffer must have a positive size!");
		ysize text_w = BufferW - (max_digs + s_LineSep.length());

		// Get line info
		const char* src;
		ysize line_len;
		std::tie(src, line_len) = file.line(idx);

		// Expand the line
		ystr ln_exp = expand_line(src, line_len, { points... });

		// Get a point iterator
		auto points_it = points_list.begin();
		ysize points_cnt = 0;

		for (ysize offs = 0; offs < ln_exp.length(); offs += text_w) {
			// Print the beginning of the line (number and separator)
			print_line_begin(offs == 0, idx, max_digs);
			// Print the part of the line
			outs << ln_exp.substr(offs, text_w) << std::endl;

			ysize prev_p = offs;
			if (points_it != points_list.end()) {
				if (*points_it < offs + text_w + points_cnt) {
					while (points_it != points_list.end()
						&& *points_it < offs + text_w + points_cnt) {
						outs
							<< ystr(*points_it - prev_p, points_cnt ? '^' : '~');
						prev_p = *points_it;
						++points_it;
						points_cnt = (points_cnt + 1) % 2;
					}
					outs << std::endl;
				}
			}
		}
	}

public:
	code_formatter() = delete; // Cannot instantiate
};
