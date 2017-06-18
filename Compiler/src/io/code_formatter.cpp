#include <cctype>
#include <algorithm>
#include "code_formatter.h"
#include "../math.h"
#include "../str_utils.h"

std::ostream*	code_formatter::Out			= &std::cout;
ysize			code_formatter::BufferW		= 20;
ysize			code_formatter::LinesBefore = 1;
ysize			code_formatter::LinesAfter	= 1;
ysize			code_formatter::MaxBetween	= 3;
ysize			code_formatter::TabSize		= 4;

ystr			code_formatter::s_LineSep		= " | ";
ystr			code_formatter::s_LineSepInt	= " ||";

// TODO: implement max between

void code_formatter::print(file_hnd const& file, interval pos, yopt<interval> pos2) {
	// Simplify access to positions
	point const& beg_pos = pos.Start;
	point const& end_pos = pos2 ? pos2->End : pos.End;

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
			print_line(file, first_annot, max_digs, { { pos.Start.Column, pos.End.Column }, { pos2->Start.Column, pos2->End.Column } });
		}
		else {
			// Single line, one arrow
			// ~~~~~~~~^^^^^^
			print_line(file, first_annot, max_digs, { { pos.Start.Column, pos.End.Column } });
		}
	}
	else {
		// Multiple annotated lines
		if (pos2) {
			print_line_beg(file, first_annot, max_digs,
			{ { pos.Start.Column, pos.End.Column } });
			for (ysize i = first_annot + 1; i < last_annot; i++) {
				print_line_in(file, i, max_digs);
			}
			print_line_end(file, last_annot, max_digs,
			{ { pos2->Start.Column, pos2->End.Column } });
		}
		else {
			print_line_beg(file, first_annot, max_digs,
			{ { pos.Start.Column, pos.Start.Column + 1 } });
			for (ysize i = first_annot + 1; i < last_annot; i++) {
				print_line_in(file, i, max_digs);
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

void code_formatter::print_line_begin(bool first, ysize idx, ysize max_digs) {
	// Get a reference to the output stream for simpler syntax
	std::ostream& outs = *Out;

	if (first) {
		// This is the first segment of the line, print line number
		// TODO: toggle '0' vs ' ' padding
		outs
			<< ystr(max_digs - math::digit_count(idx), '0')	// Padding
			<< str_utils::to_str(idx)						// Line number
			<< s_LineSep;									// Line separator
	}
	else {
		// Just skip and separate
		outs
			<< ystr(max_digs, ' ')				// Padding
			<< s_LineSep;						// Line separator
	}
}

void code_formatter::print_line_begin_in(bool first, ysize idx, ysize max_digs) {
	// Get a reference to the output stream for simpler syntax
	std::ostream& outs = *Out;

	if (first) {
		// This is the first segment of the line, print line number
		// TODO: toggle '0' vs ' ' padding
		outs
			<< ystr(max_digs - math::digit_count(idx), '0')	// Padding
			<< str_utils::to_str(idx)						// Line number
			<< s_LineSepInt;								// Line separator
	}
	else {
		// Just skip and separate
		outs
			<< ystr(max_digs, ' ')				// Padding
			<< s_LineSepInt;					// Line separator
	}
}

void code_formatter::expand_until(const char* line, ysize& start, ysize end, ystr& result) {
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

ystr code_formatter::expand_line(const char* line, ysize len, yvec<ypair<ysize, ysize>*>& points) {
	// The expanded string
	ystr result;
	// Reserve as many chars as the line length to reduce allocations
	result.reserve(len);

	ysize i = 0;
	for (auto it = points.begin(); it != points.end(); ++it) {
		// For every point, find the corresponding space
		{
			ysize& next = (*it)->first;
			expand_until(line, i, next, result);
			next = result.length();
		}
		{
			ysize& next = (*it)->second;
			expand_until(line, i, next, result);
			next = result.length();
		}
	}
	expand_until(line, i, len, result);

	return result;
}

ystr code_formatter::expand_line(const char* line, ysize len) {
	// The expanded string
	ystr result;
	// Reserve as many chars as the line length to reduce allocations
	result.reserve(len);

	ysize i = 0;
	expand_until(line, i, len, result);

	return result;
}

ystr code_formatter::generate_arrows(ysize beg, std::vector<ypair<ysize, ysize>> const& points) {
	ystr result;
	assert(points.size() && "Cannot create arrows without points!");

	result.reserve(beg + (points.rbegin()->second - points.begin()->first));
	result += ystr(beg, '_');

	auto it = points.begin();
	while (true) {
		result += ystr(it->second - it->first, '^');
		auto nit = it + 1;
		if (nit == points.end()) {
			break;
		}
		else {
			result += ystr(nit->first - it->second, '_');
			it = nit;
		}
	}

	return result;
}

void code_formatter::print_line(file_hnd const& file, ysize idx, ysize max_digs) {
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
	ystr ln_exp = expand_line(src, line_len);

	for (ysize offs = 0; offs < ln_exp.length(); offs += text_w) {
		// Print the beginning of the line (number and separator)
		print_line_begin(offs == 0, idx, max_digs);
		// Print the part of the line
		outs << ln_exp.substr(offs, text_w) << std::endl;
	}
}

void code_formatter::print_line_in(file_hnd const& file, ysize idx, ysize max_digs) {
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
	ystr ln_exp = expand_line(src, line_len);

	for (ysize offs = 0; offs < ln_exp.length(); offs += text_w) {
		// Print the beginning of the line (number and separator)
		print_line_begin_in(offs == 0, idx, max_digs);
		// Print the part of the line
		outs << ln_exp.substr(offs, text_w) << std::endl;
	}
}

void code_formatter::print_line_beg(file_hnd const& file, ysize idx, ysize max_digs, yvec<ypair<ysize, ysize>> points) {
	// Get a reference to the output stream for simpler syntax
	std::ostream& outs = *Out;

	// The actual text buffer size
	assert(BufferW > max_digs + s_LineSep.length() && "The buffer must have a positive size!");
	ysize text_w = BufferW - (max_digs + s_LineSep.length());

	// Get line info
	const char* src;
	ysize line_len;
	std::tie(src, line_len) = file.line(idx);

	yvec<ypair<ysize, ysize>*> points_ref;
	for (ypair<ysize, ysize>& el : points) {
		points_ref.push_back(&el);
	}
	// Expand the line
	ystr ln_exp = expand_line(src, line_len, points_ref);

	yopt<ystr> arrow;
	ysize arr_at = 0;
	ysize arrow_begin = points.begin()->first;

	for (ysize offs = 0; offs < ln_exp.length(); offs += text_w) {
		// Print the beginning of the line (number and separator)
		if (arrow) {
			print_line_begin_in(offs == 0, idx, max_digs);
		}
		else {
			print_line_begin(offs == 0, idx, max_digs);
		}
		// Print the part of the line
		outs << ln_exp.substr(offs, text_w) << std::endl;

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
				outs
					<< arrow->substr(arr_at, text_w)
					<< std::endl;
				arr_at += text_w;
			}
		}
	}
}

void code_formatter::print_line_end(file_hnd const& file, ysize idx, ysize max_digs, yvec<ypair<ysize, ysize>> points) {
	// Get a reference to the output stream for simpler syntax
	std::ostream& outs = *Out;

	// The actual text buffer size
	assert(BufferW > max_digs + s_LineSep.length() && "The buffer must have a positive size!");
	ysize text_w = BufferW - (max_digs + s_LineSep.length());

	// Get line info
	const char* src;
	ysize line_len;
	std::tie(src, line_len) = file.line(idx);

	yvec<ypair<ysize, ysize>*> points_ref;
	for (ypair<ysize, ysize>& el : points) {
		points_ref.push_back(&el);
	}
	// Expand the line
	ystr ln_exp = expand_line(src, line_len, points_ref);

	yopt<ystr> arrow;
	ysize arr_at = 0;
	ysize arrow_begin = points.begin()->first;

	for (ysize offs = 0; offs < ln_exp.length(); offs += text_w) {
		// Print the beginning of the line (number and separator)
		if (!arrow) {
			print_line_begin_in(offs == 0, idx, max_digs);
		}
		else {
			print_line_begin(offs == 0, idx, max_digs);
		}
		// Print the part of the line
		outs << ln_exp.substr(offs, text_w) << std::endl;

		if (!arrow && math::in_range(arrow_begin, offs, text_w)) {
			ysize beg = arrow_begin - offs;
			arrow = generate_arrows(beg, points);
		}
		if (arrow) {
			if (arr_at < arrow->length()) {
				print_line_begin_in(false, idx, max_digs);
				outs
					<< arrow->substr(arr_at, text_w)
					<< std::endl;
				arr_at += text_w;
			}
		}
	}
}

void code_formatter::print_line(file_hnd const& file, ysize idx, ysize max_digs, yvec<ypair<ysize, ysize>> points) {
	// Get a reference to the output stream for simpler syntax
	std::ostream& outs = *Out;

	// The actual text buffer size
	assert(BufferW > max_digs + s_LineSep.length() && "The buffer must have a positive size!");
	ysize text_w = BufferW - (max_digs + s_LineSep.length());

	// Get line info
	const char* src;
	ysize line_len;
	std::tie(src, line_len) = file.line(idx);

	yvec<ypair<ysize, ysize>*> points_ref;
	for (ypair<ysize, ysize>& el : points) {
		points_ref.push_back(&el);
	}
	// Expand the line
	ystr ln_exp = expand_line(src, line_len, points_ref);

	yopt<ystr> arrow;
	ysize arr_at = 0;
	ysize arrow_begin = points.begin()->first;

	for (ysize offs = 0; offs < ln_exp.length(); offs += text_w) {
		// Print the beginning of the line (number and separator)
		print_line_begin(offs == 0, idx, max_digs);

		// Print the part of the line
		outs << ln_exp.substr(offs, text_w) << std::endl;

		if (!arrow && math::in_range(arrow_begin, offs, text_w)) {
			ysize beg = arrow_begin - offs;
			arrow = generate_arrows(beg, points);
		}
		if (arrow) {
			if (arr_at < arrow->length()) {
				print_line_begin(false, idx, max_digs);
				outs
					<< arrow->substr(arr_at, text_w)
					<< std::endl;
				arr_at += text_w;
			}
		}
	}
}
