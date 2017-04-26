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
		ystr	code_printer::s_IntSeparator	= "| ";
		char	code_printer::s_ArrowLine		= '~';
		char	code_printer::s_Arrow			= '^';
		bool	code_printer::s_IntervalMode	= true;
		ysize	code_printer::s_TabSize			= 4;
		ysize	code_printer::s_MaxInterval		= 5;

		code_printer::code_printer(std::ostream& os, ysize w)
			: m_Ostream(os), m_Width(w) {
		}

		void code_printer::print(file_handle const& file, interval const& pos) {
			m_File = &file;

			// Get bounds
			ysize first_marked	= pos.Start.Row;
			ysize last_marked	= pos.End.Row;
			ysize first_before	= first_printed(first_marked);
			ysize last_after	= last_printed(last_marked);

			// Get padding and max line numbering size
			m_Padding = 0;
			if (s_LineNumbering) {
				m_DigitCount = math::digit_count(last_after);
				m_Padding = m_DigitCount + s_LineSeparator.length();
			}

			// Print lines before
			for (ysize i = first_before; i < first_marked; i++) {
				print_line(i);
			}

			// TODO: print actual line
			print_marked(first_marked, last_marked, pos.Start.Col, pos.End.Col);

			// Print lines after
			for (ysize i = last_marked + 1; i <= last_after; i++) {
				print_line(i);
			}
		}
		
		void code_printer::print_marked(ysize first, ysize last, ysize left, ysize right) {
			if (s_IntervalMode) {
				if (first == last) {
					print_line_marked(first, left, right - left);
				}
			}
			else {
				// A single pointer to the beginning
				print_line_marked(first, left, 1);

				for (ysize i = first + 1; i <= last; i++) {
					print_line(i);
				}
			}
		}

		void code_printer::print_line_marked(ysize ln_idx, ysize left, ysize arr) {
			ysize line_len = m_File->line_len(ln_idx);
			const char* src = m_File->line(ln_idx);
			bool arr_started = false;
			bool line_end = false;
			ysize arr_drawn = 0;

			const char* src2 = src;
			ysize i = 0, p = 0;

			for (ysize printed = 0; printed < line_len;) {
				print_line_padding(ln_idx, printed == 0);
				ysize part = print_part(src + printed, line_len - printed);

				if (printed + part > left && !arr_started) {
					arr_started = true;

					ysize arr_start = left - printed;
					src2 = src + printed;

					m_Ostream << fmt::skip(m_Padding);

					for (p = 0, i = 0; p < arr_start; p++) {
						if (src2[p] == '\t') {
							ysize skp = s_TabSize - (i % s_TabSize);
							m_Ostream << fmt::repeat(s_TabSize, s_ArrowLine);
							i += skp;
						}
						else {
							m_Ostream << s_ArrowLine;
							i++;
						}
					}
				}
				if (arr_started && arr_drawn < arr) {
					if (line_end) {
						m_Ostream << fmt::skip(m_Padding);
					}
					for (; i < m_Width - m_Padding && arr_drawn < arr; arr_drawn++) {
						if (src2[p] == '\t') {
							ysize skp = s_TabSize - (i % s_TabSize);
							m_Ostream << fmt::repeat(s_TabSize, s_Arrow);
							i += skp;
						}
						else {
							m_Ostream << s_Arrow;
							i++;
						}
					}
					i = 0;
					line_end = true;
					m_Ostream << std::endl;
				}

				printed += part;
			}
		}

		void code_printer::print_line(ysize ln_idx) {
			ysize line_len = m_File->line_len(ln_idx);
			const char* src = m_File->line(ln_idx);

			for (ysize printed = 0; printed < line_len;) {
				print_line_padding(ln_idx, printed == 0);
				printed += print_part(src + printed, line_len - printed);
			}
		}

		void code_printer::print_line_padding(ysize num, bool numbering) {
			if (s_LineNumbering) {
				if (numbering) {
					ysize zeroes = m_DigitCount - math::digit_count(num);
					if (s_LeadingZeroes) {
						m_Ostream 
							<< fmt::repeat(zeroes, '0') 
							<< num << s_LineSeparator;
					}
					else {
						m_Ostream
							<< fmt::skip(zeroes)
							<< num << s_LineSeparator;
					}
				}
				else {
					m_Ostream << fmt::skip(m_DigitCount) << s_LineSeparator;
				}
			}
		}

		ysize code_printer::print_part(const char* src, ysize len) {
			ysize i, p;	// i - cursor, p - printed
			for (i = 0, p = 0; p < len && i < (m_Width - m_Padding); p++) {
				char ch = src[p];
				if (ch == '\t') {
					ysize skp = s_TabSize - (i % s_TabSize);
					m_Ostream << fmt::skip(skp);
					i += skp;
				}
				else {
					i++;
					m_Ostream << ch;
				}
			}
			//if (i < (m_Width - m_Padding)) {
				m_Ostream << std::endl;
			//}
			return p;
		}

		ysize code_printer::first_printed(ysize ln) {
			// Calculate the first line to be printed
			ysize firstline = 0;
			if (s_LinesBefore <= ln) {
				firstline = ln - s_LinesBefore;
			}
			return firstline;
		}

		ysize code_printer::last_printed(ysize ln) {
			// Calculate the last line to be printed
			ysize lastline = ln + s_LinesAfter;
			if (lastline >= m_File->line_cnt()) {
				lastline = m_File->line_cnt() - 1;
			}
			return lastline;
		}
	}
}
