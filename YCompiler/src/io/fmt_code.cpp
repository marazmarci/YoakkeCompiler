#include <cctype>
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

			return std::make_tuple(begin, end);
		}

		/**
		 * Prints a line numbering or the space for the numbering followed by the
		 * separator pipes.
		 * @param first The line number is printed if true, spaces otherwise.
		 * @param idx The line number.
		 * @param max_digs The digit count of the highest printed line number.
		 */
		static void print_line_begin(bool first, ysize idx, ysize max_digs) {
			if (first) {
				// This is the first segment of the line, print line number
				// TODO: toggle '0' vs ' ' padding
				std::cout
					<< ystr(max_digs - math::digit_count(idx), '0')	// Padding
					<< std::to_string(idx)							// Line number
					<< LineSeparator;								// Line separator
			}
			else {
				// Just skip and separate
				std::cout
					<< ystr(max_digs, ' ')			// Padding
					<< LineSeparator;				// Line separator
			}
		}

		/**
		 * Prints a line numbering or the space for the numbering followed by the
		 * separator pipes when inside an interval notation.
		 * @param first The line number is printed if true, spaces otherwise.
		 * @param idx The line number.
		 * @param max_digs The digit count of the highest printed line number.
		 */
		static void print_line_begin_in(bool first, ysize idx, ysize max_digs) {
			if (first) {
				// This is the first segment of the line, print line number
				// TODO: toggle '0' vs ' ' padding
				std::cout
					<< ystr(max_digs - math::digit_count(idx), '0')	// Padding
					<< std::to_string(idx)							// Line number
					<< LineIntervalSeparator;						// Line separator
			}
			else {
				// Just skip and separate
				std::cout
					<< ystr(max_digs, ' ')			// Padding
					<< LineIntervalSeparator;		// Line separator
			}
		}

		/**
		 * A helper function for expand_line.
		 * @see expand_line
		 */
		static void expand_until(const char* line, ysize& start, ysize end, ystr& result) {
			for (; start < end; start++) {
				char c = line[start];
				if (c == '\t') {
					// Skip to the next tabbing space
					ysize skip = TabSize - result.length() % TabSize;
					result += ystr(skip, ' ');
				}
				else if (std::isprint(c)) {
					result += c;
				}
			}
		}

		/**
		 * Expands a line, removing all special characters, like tabs.
		 * @param line The pointer to the beinning of the line.
		 * @param len The length of the line.
		 * @return The expanded line string.
		 */
		static ystr expand_line(const char* line, ysize len) {
			// The expanded string
			ystr result;
			// Reserve as many chars as the line length to reduce allocations
			result.reserve(len);

			ysize i = 0;
			expand_until(line, i, len, result);

			return result;
		}

		/**
		 * Expands a line, removing all special characters, like tabs and tracks
		 * given points and their changes in space.
		 * @param line The pointer to the beinning of the line.
		 * @param len The length of the line.
		 * @param points The points that are tracked and modified to their new
		 * place accordingly.
		 * @return The expanded line string.
		 */
		static ystr expand_line(const char* line, ysize len, yvec<ytup<ysize, ysize>*>& points) {
			// The expanded string
			ystr result;
			// Reserve as many chars as the line length to reduce allocations
			result.reserve(len);

			ysize i = 0;
			for (auto it = points.begin(); it != points.end(); ++it) {
				// For every point, find the corresponding space
				{
					ysize& next = std::get<0>(**it);
					expand_until(line, i, next, result);
					next = result.length();
				}
				{
					ysize& next = std::get<1>(**it);
					expand_until(line, i, next, result);
					next = result.length();
				}
			}
			expand_until(line, i, len, result);

			return result;
		}

		/**
		 * Generates arrows pointing to the given intervals.
		 * @param beg The beginning position of the arrow.
		 * @param points The pointed intervals.
		 * @return A string containing the arrow.
		 */
		static ystr generate_arrows(ysize beg, yvec<ytup<ysize, ysize>> const& points) {
			ystr result;
			assert(points.size() && "Cannot create arrows without points!");

			result.reserve(beg + (std::get<1>(*points.rbegin()) - std::get<0>(*points.begin())));
			result += ystr(beg, '_');

			auto it = points.begin();
			while (true) {
				result += ystr(std::get<1>(*it) - std::get<0>(*it), '^');
				auto nit = it + 1;
				if (nit == points.end()) {
					break;
				}
				else {
					result += ystr(std::get<0>(*nit) - std::get<1>(*it), '_');
					it = nit;
				}
			}

			return result;
		}

		// Various print functions that have a lot of common structures
		// TODO: refactor these, lot of common code

		static void print_line(file_hnd const& file, ysize idx, ysize max_digs) {
			// The actual text buffer size
			assert(BufferWidth > max_digs + LineSeparator.length() && "The buffer must have a positive size!");
			ysize text_w = BufferWidth - (max_digs + LineSeparator.length());

			// Get line info
			const char* src;
			ysize line_len;
			std::tie(src, line_len) = file.line(idx);

			// Expand the line
			ystr ln_exp = expand_line(src, line_len);

			if (ln_exp.length()) {
				// There is something to print
				for (ysize offs = 0; offs < ln_exp.length(); offs += text_w) {
					// Print the beginning of the line (number and separator)
					print_line_begin(offs == 0, idx, max_digs);
					// Print the part of the line
					std::cout << ln_exp.substr(offs, text_w) << std::endl;
				}
			}
			else {
				// Empty line
				print_line_begin(true, idx, max_digs);
				std::cout << std::endl;
			}
		}

		static void print_line(file_hnd const& file, ysize idx, ysize max_digs, yvec<ytup<ysize, ysize>> points) {
			// The actual text buffer size
			assert(BufferWidth > max_digs + LineSeparator.length() && "The buffer must have a positive size!");
			ysize text_w = BufferWidth - (max_digs + LineSeparator.length());

			// Get line info
			const char* src;
			ysize line_len;
			std::tie(src, line_len) = file.line(idx);

			yvec<ytup<ysize, ysize>*> points_ref;
			for (ytup<ysize, ysize>& el : points) {
				points_ref.push_back(&el);
			}
			// Expand the line
			ystr ln_exp = expand_line(src, line_len, points_ref);

			yopt<ystr> arrow;
			ysize arr_at = 0;
			ysize arrow_begin = std::get<0>(*points.begin());

			assert(ln_exp.length() && "An annotated line must have a length when expanded!");
			for (ysize offs = 0; offs < ln_exp.length(); offs += text_w) {
				// Print the beginning of the line (number and separator)
				print_line_begin(offs == 0, idx, max_digs);

				// Print the part of the line
				std::cout << ln_exp.substr(offs, text_w) << std::endl;

				if (!arrow && math::in_range(arrow_begin, offs, text_w)) {
					ysize beg = arrow_begin - offs;
					arrow = generate_arrows(beg, points);
				}
				if (arrow) {
					if (arr_at < arrow->length()) {
						print_line_begin(false, idx, max_digs);
						std::cout
							<< arrow->substr(arr_at, text_w)
							<< std::endl;
						arr_at += text_w;
					}
				}
			}
		}

		static void print_line_beg(file_hnd const& file, ysize idx, ysize max_digs, yvec<ytup<ysize, ysize>> points) {
			// The actual text buffer size
			assert(BufferWidth > max_digs + LineSeparator.length() && "The buffer must have a positive size!");
			ysize text_w = BufferWidth - (max_digs + LineSeparator.length());

			// Get line info
			const char* src;
			ysize line_len;
			std::tie(src, line_len) = file.line(idx);

			yvec<ytup<ysize, ysize>*> points_ref;
			for (ytup<ysize, ysize>& el : points) {
				points_ref.push_back(&el);
			}
			// Expand the line
			ystr ln_exp = expand_line(src, line_len, points_ref);

			yopt<ystr> arrow;
			ysize arr_at = 0;
			ysize arrow_begin = std::get<0>(*points.begin());

			assert(ln_exp.length() && "An annotated line must have a length when expanded!");
			for (ysize offs = 0; offs < ln_exp.length(); offs += text_w) {
				// Print the beginning of the line (number and separator)
				if (arrow) {
					print_line_begin_in(offs == 0, idx, max_digs);
				}
				else {
					print_line_begin(offs == 0, idx, max_digs);
				}
				// Print the part of the line
				std::cout << ln_exp.substr(offs, text_w) << std::endl;

				if (!arrow && math::in_range(arrow_begin, offs, text_w)) {
					ysize beg = arrow_begin - offs;
					arrow = generate_arrows(beg, points);
				}
				if (arrow) {
					if (arr_at < arrow->length()) {
						if (arr_at == 0) {
							print_line_begin(false, idx, max_digs);
						}
						else {
							print_line_begin_in(false, idx, max_digs);
						}
						std::cout
							<< arrow->substr(arr_at, text_w)
							<< std::endl;
						arr_at += text_w;
					}
				}
			}
		}

		static void print_line_in(file_hnd const& file, ysize idx, ysize max_digs) {
			// The actual text buffer size
			assert(BufferWidth > max_digs + LineSeparator.length() && "The buffer must have a positive size!");
			ysize text_w = BufferWidth - (max_digs + LineSeparator.length());

			// Get line info
			const char* src;
			ysize line_len;
			std::tie(src, line_len) = file.line(idx);

			// Expand the line
			ystr ln_exp = expand_line(src, line_len);

			if (ln_exp.length()) {
				// There is something to print
				for (ysize offs = 0; offs < ln_exp.length(); offs += text_w) {
					// Print the beginning of the line (number and separator)
					print_line_begin_in(offs == 0, idx, max_digs);
					// Print the part of the line
					std::cout << ln_exp.substr(offs, text_w) << std::endl;
				}
			}
			else {
				// Empty line
				print_line_begin_in(true, idx, max_digs);
				std::cout << std::endl;
			}
		}

		static void print_line_end(file_hnd const& file, ysize idx, ysize max_digs, yvec<ytup<ysize, ysize>> points) {
			// The actual text buffer size
			assert(BufferWidth > max_digs + LineSeparator.length() && "The buffer must have a positive size!");
			ysize text_w = BufferWidth - (max_digs + LineSeparator.length());

			// Get line info
			const char* src;
			ysize line_len;
			std::tie(src, line_len) = file.line(idx);

			yvec<ytup<ysize, ysize>*> points_ref;
			for (ytup<ysize, ysize>& el : points) {
				points_ref.push_back(&el);
			}
			// Expand the line
			ystr ln_exp = expand_line(src, line_len, points_ref);

			yopt<ystr> arrow;
			ysize arr_at = 0;
			ysize arrow_begin = std::get<0>(*points.begin());

			assert(ln_exp.length() && "An annotated line must have a length when expanded!");
			for (ysize offs = 0; offs < ln_exp.length(); offs += text_w) {
				// Print the beginning of the line (number and separator)
				if (!arrow) {
					print_line_begin_in(offs == 0, idx, max_digs);
				}
				else {
					print_line_begin(offs == 0, idx, max_digs);
				}
				// Print the part of the line
				std::cout << ln_exp.substr(offs, text_w) << std::endl;

				if (!arrow && math::in_range(arrow_begin, offs, text_w)) {
					ysize beg = arrow_begin - offs;
					arrow = generate_arrows(beg, points);
				}
				if (arrow) {
					if (arr_at < arrow->length()) {
						print_line_begin_in(false, idx, max_digs);
						std::cout
							<< arrow->substr(arr_at, text_w)
							<< std::endl;
						arr_at += text_w;
					}
				}
			}
		}
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
			print_line(file, first_annot, max_digs, { { pos.Start.Column, pos.End.Column } });
		}
		else {
			// Multiple annotated lines
			ysize annot_cnt = last_annot - first_annot;
			print_line_beg(file, first_annot, max_digs,
			{ { pos.Start.Column, pos.Start.Column + 1 } });
			if (annot_cnt > MaxBetween) {
				std::cout
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
			{ { pos.End.Column - 1, pos.End.Column } });
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
			print_line(file, first_annot, max_digs, 
			{ { pos1.Start.Column, pos1.End.Column }, 
			{ pos2.Start.Column, pos2.End.Column } });
		}
		else {
			// Multiple annotated lines
			ysize annot_cnt = last_annot - first_annot;
			print_line_beg(file, first_annot, max_digs,
			{ { pos1.Start.Column, pos1.End.Column } });
			if (annot_cnt > MaxBetween) {
				std::cout
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
			{ { pos2.Start.Column, pos2.End.Column } });
		}

		for (ysize i = last_annot + 1; i <= last; i++) {
			// Print the lines after the annotated lines
			print_line(file, i, max_digs);
		}
	}
}