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

ystr			code_formatter::s_LineSep		= " | ";
ystr			code_formatter::s_LineSepInt	= " ||";

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
		print_line(file, i, max_digs, arrow_state::None);
	}

	if (first_annot == last_annot) {
		// Single-line annotation
		if (pos2) {
			// Single line, two arrows
			// ~~~~~~~~^^^^^^~~~~~~~~^^^^^^
			print_line(file, first_annot, max_digs, arrow_state::None, pos.Start.Column, pos.End.Column, pos2->Start.Column, pos2->End.Column);
		}
		else {
			// Single line, one arrow
			// ~~~~~~~~^^^^^^
			print_line(file, first_annot, max_digs, arrow_state::None, pos.Start.Column, pos.End.Column);
		}
	}
	else {
		// Multiple annotated lines
		if (pos2) {
			print_line(file, first_annot, max_digs, arrow_state::Begin,
				pos.Start.Column, pos.End.Column);
			for (ysize i = first_annot + 1; i < last_annot; i++) {
				print_line(file, i, max_digs, arrow_state::In);
			}
			print_line(file, last_annot, max_digs, arrow_state::End,
				pos2->Start.Column, pos2->End.Column);
		}
		else {
			print_line(file, first_annot, max_digs, arrow_state::Begin,
				pos.Start.Column, pos.Start.Column + 1);
			for (ysize i = first_annot + 1; i < last_annot; i++) {
				print_line(file, i, max_digs, arrow_state::In);
			}
			print_line(file, last_annot, max_digs, arrow_state::End,
				pos.End.Column, pos.End.Column + 1);
		}
	}

	for (ysize i = last_annot + 1; i <= last; i++) {
		// Print the lines after the annotated lines
		print_line(file, i, max_digs, arrow_state::None);
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

void code_formatter::print_line_begin(bool first, ysize idx, ysize max_digs, bool in) {
	// Get a reference to the output stream for simpler syntax
	std::ostream& outs = *Out;

	if (first) {
		// This is the first segment of the line, print line number
		// TODO: toggle '0' vs ' ' padding
		outs
			<< ystr(max_digs - digit_count_of(idx), '0')	// Padding
			<< str_utils::to_str(idx)						// Line number
			<< (in ? s_LineSepInt : s_LineSep);				// Line separator
	}
	else {
		// Just skip and separate
		outs
			<< ystr(max_digs, ' ')				// Padding
			<< (in ? s_LineSepInt : s_LineSep);	// Line separator
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

ystr code_formatter::generate_arrows(ysize beg, std::initializer_list<ysize> points) {
	ystr result;
	assert(points.size() && "Cannot create arrows without points!");
	assert(points.size() % 2 == 0 && "Cannot create arrows from odd number of points!");

	ysize cnt = 0;
	result.reserve(beg + *(points.end() - 1) - *points.begin());
	result += ystr(beg, '_');

	auto it = points.begin();
	auto itp1 = it + 1;
	
	while (itp1 != points.end()) {
		result += ystr(*itp1 - *it, cnt % 2 ? '_' : '^');
		it = itp1++;
		cnt++;
	}

	return result;
}
