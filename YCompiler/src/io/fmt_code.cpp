#include <iostream>
#include "fmt_code.h"
#include "../math.h"

namespace fmt_code {
	ysize BufferWidth	= 120;
	ysize LinesBefore	= 1;
	ysize LinesAfter	= 1;
	ysize MaxBetween	= 3;
	ysize TabSize		= 4;

	namespace {
		ystr LineSeparator = " | ";
		ystr LineIntervalSeparator = " ||";

		/**
		 * Calculates the first and the last line indicies that should be printed
		 * based on the file and the annotated position.
		 * @param file The souce file.
		 * @param from The first annotated line.
		 * @param to The last annotated line.
		 * @return A pair of indicies in the form of (from, to).
		 */
		static ytup<ysize, ysize> get_bounds(file_hnd const& file, ysize from, ysize to) {
			// First line
			ysize begin;
			if (from < LinesBefore) {
				begin = 0;
			}
			else {
				begin = from - LinesBefore;
			}
			// Last line
			ysize end;
			if (to + LinesAfter >= file.line_count()) {
				assert(file.line_count() && "File must have at least one line!");
				end = file.line_count() - 1;
			}
			else {
				end = to + LinesAfter;
			}

			return { begin, end };
		}

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
		static ystr generate_arrows(ysize beg, yvec<ytup<ysize, ysize>> const& points);

		// Various print functions that have a lot of common structures
		// TODO: refactor these, lot of common code
		static void print_line(file_hnd const& file, ysize idx, ysize max_digs);
		static void print_line(file_hnd const& file, ysize idx, ysize max_digs, yvec<ytup<ysize, ysize>> points);
		static void print_line_beg(file_hnd const& file, ysize idx, ysize max_digs, yvec<ytup<ysize, ysize>> points);
		static void print_line_in(file_hnd const& file, ysize idx, ysize max_digs);
		static void print_line_end(file_hnd const& file, ysize idx, ysize max_digs, yvec<ytup<ysize, ysize>> points);
	}

	void print(file_hnd const& file, interval const& pos) {
		// Simplify access to positions
		point const& beg_pos = pos.Start;
		point const& end_pos = pos.End;

		// First and last annotated
		ysize first_annot = beg_pos.Row;
		ysize last_annot = end_pos.Row;

		// Get the first and last line index that gets printed
		ysize first, last;
		std::tie(first, last) = get_bounds(file, first_annot, last_annot);

		ysize max_digs = math::digit_count(last);

		for (ysize i = first; i < first_annot; i++) {
			// Print the lines before the annotated lines
			print_line(file, i, max_digs);
		}

		if (first_annot == last_annot) {
			// Single-line annotation
			if (pos2) {
				// Single line, two arrows
				// ~~~~~~~~^^^^^^~~~~~~~~^^^^^^
				print_line(file, first_annot, max_digs, { { pos.Start.Column, pos.End.Column },{ pos2->Start.Column, pos2->End.Column } });
			}
			else {
				// Single line, one arrow
				// ~~~~~~~~^^^^^^
				print_line(file, first_annot, max_digs, { { pos.Start.Column, pos.End.Column } });
			}
		}
		else {
			// Multiple annotated lines
			ysize annot_cnt = last_annot - first_annot;
			if (pos2) {
				print_line_beg(file, first_annot, max_digs,
				{ { pos.Start.Column, pos.End.Column } });
				if (annot_cnt > MaxBetween) {
					outs
						<< ystr(max_digs, ' ')
						<< "..."
						<< std::endl;
				}
				else {
					for (ysize i = first_annot + 1; i < last_annot; i++) {
						print_line_in(file, i, max_digs);
					}
				}
				print_line_end(file, last_annot, max_digs,
				{ { pos2->Start.Column, pos2->End.Column } });
			}
			else {
				print_line_beg(file, first_annot, max_digs,
				{ { pos.Start.Column, pos.Start.Column + 1 } });
				if (annot_cnt > MaxBetween) {
					outs
						<< ystr(max_digs, ' ')
						<< "..."
						<< std::endl;
				}
				else {
					for (ysize i = first_annot + 1; i < last_annot; i++) {
						print_line_in(file, i, max_digs);
					}
				}
				print_line_end(file, last_annot, max_digs,
				{ { pos.End.Column, pos.End.Column + 1 } });
			}
		}

		for (ysize i = last_annot + 1; i <= last; i++) {
			// Print the lines after the annotated lines
			print_line(file, i, max_digs);
		}
	}

	void print(file_hnd const& file, interval const& pos1, interval const& pos2) {
		// Simplify access to positions
		point const& beg_pos = pos1.Start;
		point const& end_pos = pos2.End;

		// First and last annotated
		ysize first_annot = beg_pos.Row;
		ysize last_annot = end_pos.Row;

		// Get the first and last line index that gets printed
		ysize first, last;
		std::tie(first, last) = get_bounds(file, first_annot, last_annot);

		ysize max_digs = math::digit_count(last);

		for (ysize i = first; i < first_annot; i++) {
			// Print the lines before the annotated lines
			print_line(file, i, max_digs);
		}

		if (first_annot == last_annot) {
			// Single-line annotation
			if (pos2) {
				// Single line, two arrows
				// ~~~~~~~~^^^^^^~~~~~~~~^^^^^^
				print_line(file, first_annot, max_digs, { { pos.Start.Column, pos.End.Column },{ pos2->Start.Column, pos2->End.Column } });
			}
			else {
				// Single line, one arrow
				// ~~~~~~~~^^^^^^
				print_line(file, first_annot, max_digs, { { pos.Start.Column, pos.End.Column } });
			}
		}
		else {
			// Multiple annotated lines
			ysize annot_cnt = last_annot - first_annot;
			if (pos2) {
				print_line_beg(file, first_annot, max_digs,
				{ { pos.Start.Column, pos.End.Column } });
				if (annot_cnt > MaxBetween) {
					outs
						<< ystr(max_digs, ' ')
						<< "..."
						<< std::endl;
				}
				else {
					for (ysize i = first_annot + 1; i < last_annot; i++) {
						print_line_in(file, i, max_digs);
					}
				}
				print_line_end(file, last_annot, max_digs,
				{ { pos2->Start.Column, pos2->End.Column } });
			}
			else {
				print_line_beg(file, first_annot, max_digs,
				{ { pos.Start.Column, pos.Start.Column + 1 } });
				if (annot_cnt > MaxBetween) {
					outs
						<< ystr(max_digs, ' ')
						<< "..."
						<< std::endl;
				}
				else {
					for (ysize i = first_annot + 1; i < last_annot; i++) {
						print_line_in(file, i, max_digs);
					}
				}
				print_line_end(file, last_annot, max_digs,
				{ { pos.End.Column, pos.End.Column + 1 } });
			}
		}

		for (ysize i = last_annot + 1; i <= last; i++) {
			// Print the lines after the annotated lines
			print_line(file, i, max_digs);
		}
	}
}