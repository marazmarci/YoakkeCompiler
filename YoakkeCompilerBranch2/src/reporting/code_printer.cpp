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
		ystr	code_printer::s_LineSeparator	= " |  ";
		ystr	code_printer::s_IntSeparator	= " | |";
		char	code_printer::s_ArrowLine		= '~';
		char	code_printer::s_Arrow			= '^';
		bool	code_printer::s_IntervalMode	= false;
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
			//print_marked(first_marked, last_marked, pos.Start.Col, pos.End.Col);

			// Print lines after
			for (ysize i = last_marked + 1; i <= last_after; i++) {
				print_line(i);
			}
		}
		
		void code_printer::print_line(ysize ln_idx) {
			ysize line_len = m_File->line_len(ln_idx);
			const char* src = m_File->line(ln_idx);

			print_line_padding(ln_idx, true);
			ysize printed = print_part(src, line_len);

			while (printed < line_len) {
				print_line_padding(ln_idx, false);
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
			if (i < (m_Width - m_Padding)) {
				m_Ostream << std::endl;
			}
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
