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

void code_formatter::print(file_hnd const& file, interval const& pos) {
	// Get easy access for the positions
	position const& pos1 = pos.Start;
	position const& pos2 = pos.End;

	// Get the first printed line in bounds
	ysize first_ln = pos1.Row < LinesBefore ? 0 : pos1.Row - LinesBefore;
	// Get the last printed line in bounds
	ysize last_ln;
	{
		ysize max_ln = file.line_count() - 1;
		ysize max_sum = pos2.Row + LinesAfter;
		last_ln = max_sum > max_ln ? max_ln : max_sum;
	}
	
	// Get the first and last annotated lines
	ysize first_annot = pos1.Row;
	ysize last_annot = pos2.Row;

	// Get the number of digits of the largest line number
	ysize digit_cnt = digit_count_of(last_ln);

	// Print the lines before
	for (ysize idx = first_ln; idx < first_annot; idx++) {
		print_line(file, idx, digit_cnt);
	}

	if (first_annot != last_annot) {
		// Multi-line annotation

	}
	else {
		// Single-line annotation
		print_line_annot(file, first_annot, digit_cnt, pos1.Column, pos2.Column);
	}

	// Print the lines after
	for (ysize idx = last_annot + 1; idx <= last_ln; idx++) {
		print_line(file, idx, digit_cnt);
	}
}

void code_formatter::print_line(file_hnd const& file, ysize idx, ysize digit_cnt) {
	// Get the line source and line length
	const char* line_src;
	ysize		line_len;
	std::tie(line_src, line_len) = file.line(idx);

	// Offset is how much we have printed of the line
	for (ysize offset = 0; offset < line_len; 
		offset += print_line_segment(line_src, idx, offset, line_len, digit_cnt));
}

void code_formatter::print_line_annot(file_hnd const& file, ysize idx, ysize digit_cnt, ysize start, ysize end) {
	// Get a reference to the output stream for nicer syntax
	std::ostream& outs = *Out;
	
	// The state of the arrow
	// None: did not reach the first section
	// Head: draw arrow head
	// End: done drawing
	enum {
		None, Head, End
	} state = None;

	// Get the line source and line length
	const char* line_src;
	ysize		line_len;
	std::tie(line_src, line_len) = file.line(idx);

	// TODO: arrow head starts at same line as ~ ends!

	// Offset is how much we have printed of the line
	for (ysize offset = 0; offset < line_len;) {
		// Print and get how many chars we have outputted
		ysize delta = print_line_segment(line_src, idx, offset, line_len, digit_cnt, yvec<yref<ysize>>{ start, end });
		if (state == None && offset + delta >= start) {
			// Have not started the arrow yet
			// Print beginning of line
			outs
				<< ystr(digit_cnt + s_LineSep.length(), ' ')
				<< ystr(start - offset, '~');
			// Go into line head drawing state
			state = Head;
		}
		if (state == Head) {
			// Get how much we can print of the arrow head
			ysize head_len = std::min(offset + delta, end);
			// Print arrow head
			outs
				<< ystr(head_len - offset, '^') << std::endl;
			if (head_len == end) {
				state = End;
			}
		}
		offset += delta;
	}
}

ysize code_formatter::print_line_segment(const char* src, ysize idx, ysize offs, ysize len, ysize digit_cnt, yvec<yref<ysize>>& points) {
	// Get a reference to the output stream for nicer syntax
	std::ostream& outs = *Out;

	// Print the line beginning (the numbering and line separator)
	print_line_number(offs == 0, idx, digit_cnt);

	// Calculate text buffer width, some space is needed for line numbers
	ysize text_w = BufferW - (s_LineSep.length() + digit_cnt);

	// Point to the part's beginning
	src += offs;

	ysize printed = 0;	// The printed characters
	ysize moved = 0;	// The cursor movement

	auto point_it = points.begin();	// Where in the point vector are we

	// Print characters until the cursor exceeds the text buffer width
	// or we have reached the end of line
	for (; moved < text_w && printed + offs < len; printed++, src++) {
		if (*src == '\t') {
			// How many chars to skip
			ysize skip = TabSize - moved % TabSize;
			// Skip that much
			outs << ystr(skip, ' ');
			// The cursor moved that much we have skipped
			moved += skip;
			// Check if we have reached a point
			if (point_it != points.end()) {
				auto p = *point_it;
				if (p.get() == printed) {
					// We have reached an annotated point
					p.get() = moved;
					// Go to track the next point
					++point_it;
				}
			}
		}
		else {
			// Just print the char
			outs << *src;
			// Moved one character
			moved++;
		}
	}
	// End the line
	outs << std::endl;

	return printed;
}

void code_formatter::print_line_number(bool first, ysize num, ysize digit_cnt) {
	// Get a reference to the output stream for nicer syntax
	std::ostream& outs = *Out;

	if (first) {
		// This is the first line, write the line number
		// Get current line's digit count
		ysize digs = digit_count_of(num);

		// Write leading zeroes
		// TODO: toggle zeroes or spaces?
		outs << ystr(digit_cnt - digs, '0');

		// Print the line number itself and the separator
		outs << num << s_LineSep;
	}
	else {
		// Not the first line, just skip as nesecarry and separate
		outs << ystr(digit_cnt, ' ') << s_LineSep;
	}
}
