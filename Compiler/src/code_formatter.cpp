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

void code_formatter::print_line(file_hnd const& file, ysize idx, ysize max_digs) {
	// Get a reference to the output stream for simpler syntax
	std::ostream& outs = *Out;

	// Get line info
	const char* src;
	ysize line_len;
	std::tie(src, line_len) = file.line(idx);

	// offs is where we are in the line
	for (ysize offs = 0; offs < line_len; 
		offs += print_line_sect(idx, src + offs, offs, line_len, max_digs));
}

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

ysize code_formatter::print_line_sect(ysize idx, const char* src, ysize offs, ysize line_len, ysize max_digs) {
	// Get a reference to the output stream for simpler syntax
	std::ostream& outs = *Out;

	// The actual text buffer size
	assert(BufferW > max_digs + s_LineSep.length() && "The buffer must have a positive size!");
	ysize text_w = BufferW - (max_digs + s_LineSep.length());

	ysize printed = 0;	// How many chars are printed
	ysize moved = 0;	// How many characters the cursor has moved

	// Print the beginning of the line
	print_line_begin(offs == 0, idx, max_digs);

	for (; moved < text_w && offs + printed < line_len; printed++) {
		if (src[printed] == '\t') {
			// Calculate how many characters a tab skips
			ysize skip = TabSize - moved % TabSize;
			// Skip that many chars
			outs << ystr(skip, ' ');
			// We have moved that much
			moved += skip;
		}
		else {
			outs << src[printed];
			moved++;
		}
	}
	// End the line
	outs << std::endl;
	
	if (src[printed] == '\r') {
		// Fix for carriage return character
		printed++;
	}
	return printed;
}
