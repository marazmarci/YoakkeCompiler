#include <cctype>
#include <algorithm>
#include "code_formatter.h"
#include "str_utils.h"

std::ostream*	code_formatter::Out			= &std::cout;
ysize			code_formatter::BufferW		= 20;
ysize			code_formatter::LinesBefore = 1;
ysize			code_formatter::LinesAfter	= 1;
ysize			code_formatter::MaxBetween	= 3;
ysize			code_formatter::TabSize		= 4;

ystr			code_formatter::s_LineSep	= " | ";

/**
 * Helper function that calculate the amount of digits needed to print a 
 * number.
 * @param num The number to calculate the length of.
 * @return The number of digits of the number.
 */
static inline ysize digit_count_of(ysize num) {
	return str_utils::to_str(num).length();
}

void code_formatter::print(file_hnd const& file, interval pos, yopt<interval> pos2) {
	// Simplify access to positions
	position const& beg_pos = pos.Start;
	position const& end_pos = pos2 ? pos2->End : pos.End;

	// First and last annotated
	ysize first_annot = beg_pos.Row;
	ysize last_annot = end_pos.Row;

	// Get the first and last line index that gets printed
	ysize first, last;
	std::tie(first, last) = get_bounds(file, first_annot, last_annot);

	ysize max_digs = digit_count_of(last);

	for (ysize i = first; i < first_annot; i++) {
		// Print the lines before the annotated lines
		print_line(file, i, max_digs);
	}

	if (first_annot == last_annot) {
		// Single-line annotation
		if (pos2) {
			// Single line, two arrows
			// ~~~~~~~~^^^^^^~~~~~~~~^^^^^^
		}
		else {
			// Single line, one arrow
			// ~~~~~~~~^^^^^^
			print_line(file, first_annot, max_digs, pos.Start.Column, pos.End.Column);
		}
	}
	else {
		// Multiple annotated lines
		
	}

	for (ysize i = last_annot + 1; i <= last; i++) {
		// Print the lines after the annotated lines
		print_line(file, i, max_digs);
	}
}

ypair<ysize, ysize> code_formatter::get_bounds(file_hnd const& file, ysize from, ysize to) {
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

//void code_formatter::print_line(file_hnd const& file, ysize idx, ysize max_digs) {
//	// Get a reference to the output stream for simpler syntax
//	std::ostream& outs = *Out;
//
//	// The actual text buffer size
//	assert(BufferW > max_digs + s_LineSep.length() && "The buffer must have a positive size!");
//	ysize text_w = BufferW - (max_digs + s_LineSep.length());
//
//	// Get line info
//	const char* src;
//	ysize line_len;
//	std::tie(src, line_len) = file.line(idx);
//
//	// Expand the line
//	ystr ln_exp = expand_line(src, line_len);
//
//	for (ysize offs = 0; offs < ln_exp.length(); offs += text_w) {
//		// Print the beginning of the line (number and separator)
//		print_line_begin(offs == 0, idx, max_digs);
//		// Print the part of the line
//		outs << ln_exp.substr(offs, text_w) << std::endl;
//	}
//}

void code_formatter::print_line_begin(bool first, ysize idx, ysize max_digs) {
	// Get a reference to the output stream for simpler syntax
	std::ostream& outs = *Out;

	if (first) {
		// This is the first segment of the line, print line number
		// TODO: toggle '0' vs ' ' padding
		outs
			<< ystr(max_digs - digit_count_of(idx), '0')	// Padding
			<< str_utils::to_str(idx)						// Line number
			<< s_LineSep;									// Line separator
	}
	else {
		// Just skip and separate
		outs
			<< ystr(max_digs, ' ')	// Padding
			<< s_LineSep;			// Line separator
	}
}

ystr code_formatter::expand_line(const char* line, ysize len, std::initializer_list<yref<ysize>> points) {
	// The expanded string
	ystr result;
	// Reserve as many chars as the line length to reduce allocations
	result.reserve(len);

	ysize i = 0;
	for (auto it = points.begin(); it != points.end(); ++it) {
		// For every point, find the corresponding space
		ysize next = it->get();
		for (; i < next; i++) {
			char c = line[i];
			if (c == '\t') {
				// Skip to the next tabbing space
				ysize skip = TabSize - result.length() % TabSize;
				result += ystr(skip, ' ');
			}
			else if (std::isprint(c)) {
				result += c;
			}
		}
		it->get() = result.length();
	}
	for (; i < len; i++) {
		char c = line[i];
		if (c == '\t') {
			// Skip to the next tabbing space
			ysize skip = TabSize - result.length() % TabSize;
			result += ystr(skip, ' ');
		}
		else if (std::isprint(c)) {
			result += c;
		}
	}

	return result;
}