#include "code_printer.h"
#include "../utility/fmt_out.h"
#include "../utility/math.h"

namespace yk {
	namespace rep {
		ypair<ysize, ysize> mark_buffer::set(const char* str, ysize str_len, ysize x, ysize y) {
			m_Top.reserve(str_len);
			ysize i = 0;
			for (; i < x; i++) {
				char c = str[i];
				if (c == '\t') {
					for (ysize j = 0; j < code_printer::s_TabSize; j++) {
						m_Top += ' ';
					}
					x += code_printer::s_TabSize - 1;
					y += code_printer::s_TabSize - 1;
				}
				else {
					m_Top += c;
				}
			}
			for (; i < y; i++) {
				char c = str[i];
				if (c == '\t') {
					for (ysize j = 0; j < code_printer::s_TabSize; j++) {
						m_Top += ' ';
					}
					y += code_printer::s_TabSize - 1;
				}
				else {
					m_Top += c;
				}
			}
			for (; i < str_len; i++) {
				char c = str[i];
				if (c == '\t') {
					for (ysize j = 0; j < code_printer::s_TabSize; j++) {
						m_Top += ' ';
					}
				}
				else {
					m_Top += c;
				}
			}
			return  { x, y };
		}

		void mark_buffer::point_at(ysize x, ysize y) {
			m_Bottom.reserve(m_Top.length());
			ysize i = 0;
			for (; i < x; i++) {
				m_Bottom += '~';
			}
			for (; i < y; i++) {
				m_Bottom += '^';
			}
		}

		void mark_buffer::print(ysize line, ysize maxdigit, ysize buff_width) {
			static const ystr number_sep = " | ";
			static const ysize sep_len = number_sep.length();

			ysize padding = maxdigit + sep_len;
			buff_width -= padding;

			{
				// First needs number
				ysize digit_cnt = math::digit_count(line);
				*code_printer::s_Ostream 
					<< fmt::repeat(maxdigit - digit_cnt, '0')
					<< line << number_sep;
				for (ysize i = 0; i < buff_width && i < m_Top.length(); i++) {
					*code_printer::s_Ostream 
						<< m_Top[i];
				}
				*code_printer::s_Ostream
					<< std::endl;
				for (ysize offs = buff_width; offs < m_Top.length(); offs += buff_width) {
					// Rest just blank
					*code_printer::s_Ostream
						<< fmt::skip(maxdigit) << number_sep;
					for (ysize i = offs; i < offs + buff_width &&
						i < m_Top.length(); i++) {
						*code_printer::s_Ostream
							<< m_Top[i];
					}
					*code_printer::s_Ostream
						<< std::endl;
				}
			}
		}

		//////////////////////////////////////////////////////////

		ysize				code_printer::s_BufferWidth	= 30;
		ysize				code_printer::s_TabSize		= 4;
		std::ostream*		code_printer::s_Ostream		= &std::cout;
		ysize				code_printer::s_LinesBefore = 0;
		ysize				code_printer::s_LinesAfter	= 0;
		interval			code_printer::s_ErrorPos;
		file_handle const*	code_printer::s_File;

		void code_printer::print(file_handle const& file, interval const& pos) {
			s_File = &file;
			s_ErrorPos = pos;

			print_marked(pos.Start.Row, pos.End.Row, pos.Start.Col, pos.End.Col);
		}

		void code_printer::print_marked(ysize from, ysize to, ysize left, ysize right) {
			ysize first = first_printed(from);
			ysize last = first_printed(to);
			
			if (from == to) {
				const char* src = s_File->line(from);
				ysize line_len = s_File->line_len(from);

				mark_buffer lined_row;
				std::tie(left, right) = lined_row.set(src, line_len, left, right);
				lined_row.point_at(left, right);
				lined_row.print(from, math::digit_count(last), s_BufferWidth);

			}
			else {

			}
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
			if (lastline >= s_File->line_cnt()) {
				lastline = s_File->line_cnt() - 1;
			}
			return lastline;
		}
	}
}
