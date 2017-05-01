#include <algorithm>
#include "code_printer.h"
#include "../utility/fmt_out.h"
#include "../utility/math.h"

namespace yk {
	namespace rep {
		ypair<ysize, ysize> mark_buffer::set(const char* str, ysize str_len, ysize x, ysize y) {
			ysize newx = x;
			ysize newy = y;

			m_Top.reserve(str_len);

			ysize i = 0;
			for (; i < x; i++) {
				if (consume(str[i])) {
					newx += code_printer::s_TabSize - 1;
					newy += code_printer::s_TabSize - 1;
				}
			}
			for (; i < y; i++) {
				if (consume(str[i])) {
					newy += code_printer::s_TabSize - 1;
				}
			}
			for (; i < str_len; i++) {
				consume(str[i]);
			}
			return  { newx, newy };
		}

		void mark_buffer::point_at(ysize x, ysize y) {
			m_Begin = x;
			m_End = y;
		}

		// TODO:
		// Recalculate padding, add extra ~~, single line case fix
		void mark_buffer::print(ysize line, ysize maxdigit, ysize buff_width, interval_enclose enclose) {
			static const ystr number_sep = " | ";
			static const ystr int_sep = "| ";
			static const ysize sep_len = number_sep.length();
			static const ysize isep_len = int_sep.length();
			std::ostream& os = *code_printer::s_Ostream;

			ysize padding = maxdigit + sep_len;
			buff_width -= padding;
			ysize digit_cnt = math::digit_count(line);
			bool in_interval = enclose == interval_enclose::Bottom;
			if (in_interval) {
				padding += isep_len;
				buff_width -= isep_len;
			}

			ysize written = 0;
			for (ysize offs = 0; offs < m_Top.length(); offs = written) {
				// First needs number, the rest blank
				if (offs) {
					os << fmt::skip(maxdigit) << number_sep;
				}
				else {
					os << fmt::repeat(maxdigit - digit_cnt, '0')
						<< line << number_sep;
				}
				if (in_interval) {
					os << int_sep;
				}

				written = std::min(offs + buff_width, m_Top.length());
				for (ysize i = offs; i < written; i++) {
					os << m_Top[i];
				}
				os << std::endl;

				}
					os
						<< fmt::skip(padding)
						<< std::endl;
				}
			}
		}

		bool mark_buffer::consume(char c) {
			if (c == '\t') {
				for (ysize j = 0; j < code_printer::s_TabSize; j++) {
					m_Top += ' ';
				}
				return true;
			}
			else {
				m_Top += c;
				return false;
			}
		}

		//////////////////////////////////////////////////////////

		ysize				code_printer::s_BufferWidth	= 20;
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
				print_marked_single(from, math::digit_count(last), left, right, interval_enclose::None);
			}
			else {
				print_marked_single(from, math::digit_count(last), left, left + 1, interval_enclose::Top);
				print_marked_single(to, math::digit_count(last), right - 1, right, interval_enclose::Bottom);
			}
		}

		void code_printer::print_marked_single(ysize ln_idx, ysize maxdig, ysize left, ysize right, interval_enclose enclose) {
			const char* src = s_File->line(ln_idx);
			ysize line_len = s_File->line_len(ln_idx);

			mark_buffer lined_row;
			std::tie(left, right) = lined_row.set(src, line_len, left, right);
			lined_row.point_at(left, right);
			lined_row.print(ln_idx, maxdig, s_BufferWidth, enclose);
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
