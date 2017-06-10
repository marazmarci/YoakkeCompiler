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

	static void print_line_begin(bool first, ysize idx, ysize max_digs, bool in = false);
	static ystr expand_line(const char* line, ysize len, std::initializer_list<yref<ysize>> points = {});
	static ystr generate_arrows(ysize beg, std::initializer_list<ysize> points);

private:
	template <typename... Ts>
	static void print_line(file_hnd const& file, ysize idx, ysize max_digs, bool in, Ts... points) {
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

		// Create an initializer list from the points so we can iterate over them
		std::initializer_list<ysize> points_list = { points... };

		if (points_list.size()) {
			bool created = false;
			ystr arrow;
			ysize arr_at = 0;

			for (ysize offs = 0; offs < ln_exp.length(); offs += text_w) {
				// Print the beginning of the line (number and separator)
				print_line_begin(offs == 0, idx, max_digs, in);
				// Print the part of the line
				outs << ln_exp.substr(offs, text_w) << std::endl;

				if (!created && offs + text_w > *points_list.begin()
					&& *points_list.begin() >= offs) {
						created = true;
						ysize beg = *points_list.begin() - offs;
						arrow = generate_arrows(beg, points_list);
				}
				if (created) {
					if (arr_at < arrow.length()) {
						outs
							<< ystr(max_digs + s_LineSep.length(), ' ')
							<< arrow.substr(arr_at, text_w)
							<< std::endl;
						arr_at += text_w;
					}
				}
			}
		}
		else {
			for (ysize offs = 0; offs < ln_exp.length(); offs += text_w) {
				// Print the beginning of the line (number and separator)
				print_line_begin(offs == 0, idx, max_digs, in);
				// Print the part of the line
				outs << ln_exp.substr(offs, text_w) << std::endl;
			}
		}
	}

public:
	code_formatter() = delete; // Cannot instantiate
};
