#include "code_printer.h"
#include "../utility/fmt_out.h"
#include "../utility/math.h"
#include "../utility/console.h"

namespace yk {
	namespace rep {
		ysize	code_printer::s_LinesBefore		= 1;
		ysize	code_printer::s_LinesAfter		= 1;
		bool	code_printer::s_LineNumbering	= true;
		bool	code_printer::s_LeadingZeroes	= true;
		ystr	code_printer::s_LineSeparator	= " | ";
		char	code_printer::s_ArrowLine		= '~';
		char	code_printer::s_Arrow			= '^';
		bool	code_printer::s_ArrowAbove		= true;
		bool	code_printer::s_IntervalMode	= true;

		void code_printer::print(std::ostream& os, file_handle const& file, interval const& pos) {
			// Calculate the last line to be printed
			ysize lastline = pos.End.Row + s_LinesAfter;
			if (lastline >= file.line_cnt()) {
				lastline = file.line_cnt() - 1;
			}
			
			// Determine line beginning
			ysize line_numbering = 0;
			ysize line_padding = 0;
			if (s_LineNumbering) {
				line_numbering = math::digit_count(lastline - 1);
				line_padding = line_numbering + s_LineSeparator.length();
			}

			// Print previous lines
			ysize firstline = 0;
			if (s_LinesBefore <= pos.Start.Row) {
				firstline = pos.Start.Row - s_LinesBefore;
			}
			for (ysize i = firstline; i < pos.Start.Row; i++) {
				print_line(os, file, i, line_numbering, line_padding);
			}

			// Print the ACTUAL line
			print_line(os, file, pos.Start.Row, line_numbering, line_padding);

			// Print following lines
			for (ysize i = pos.End.Row + 1; i <= lastline; i++) {
				print_line(os, file, i, line_numbering, line_padding);
			}
		}

		void code_printer::print_line(std::ostream& os, file_handle const& file, ysize ln, ysize max_num_len, ysize pad) {
			ysize line_len = file.line_len(ln);
			const char* src = file.line(ln);
			ysize rem_len = console::Width - pad;
			ysize printed = 0;
			ysize cur_pos;
			while (printed < line_len) {
				if (pad) {
					print_line_begin(os, ln, max_num_len, printed == 0);
				}
				for (cur_pos = 0; cur_pos < rem_len && printed < line_len;) {
					char curr_char = src[printed++];
					if (curr_char == '\t') {
						ysize skp = console::TabSize - (cur_pos % console::TabSize);
						cur_pos += skp;
						os << fmt::skip(skp);
					}
					else {
						cur_pos++;
						os << curr_char;
					}
				}
				if (cur_pos < rem_len) {
					os << std::endl;
				}
			}
		}

		void code_printer::print_line_begin(std::ostream& os, ysize ln, ysize max_num_len, bool first) {
			if (first) {
				ysize ln_len = math::digit_count(ln);
				if (s_LeadingZeroes) {
					os << fmt::repeat(max_num_len - ln_len, '0');
				}
				else {
					os << fmt::skip(max_num_len - ln_len);
				}
				os << ln;
			}
			else {
				os << fmt::skip(max_num_len);
			}
			os << s_LineSeparator;
		}
	}
}
