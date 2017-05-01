#include "code_printer.h"

namespace yk {
	namespace rep {
		ysize				code_printer::s_BufferWidth		= 20;
		ysize				code_printer::s_TabSize			= 4;
		ysize				code_printer::s_LinesBefore		= 0;
		ysize				code_printer::s_LinesAfter		= 0;
		bool				code_printer::s_LineNumbering	= true;
		ystr				code_printer::s_NumberSep		= " | ";
		ystr				code_printer::s_IntervalSep		= "| ";
		char				code_printer::s_LineBase		= '~';
		char				code_printer::s_LineHead		= '^';
		std::ostream*		code_printer::s_Ostream			= &std::cout;
		file_handle const*	code_printer::s_File;

		void code_printer::print(file_handle const& file, interval const& pos) {
			s_File = &file;
			print_marked(pos.Start.Row, pos.End.Row, pos.Start.Col, pos.End.Col);
		}
		
		void code_printer::print_marked(ysize from, ysize to, ysize left, ysize right) {
			ysize first = first_printed(from);
			ysize last = first_printed(to);
			
			if (from == to) {
				print_marked_single(from, math::digit_count(last), left, right, enclose_t::None);
			}
			else {
				print_marked_single(from, math::digit_count(last), left, left + 1, enclose_t::Top);
				print_marked_single(to, math::digit_count(last), right - 1, right, enclose_t::Bottom);
			}
		}

		void code_printer::print_marked_single(ysize ln_idx, ysize maxdig, ysize left, ysize right, enclose_t enclose) {
			mark_buffer<true, false> buff;
			buff.set(*s_File, ln_idx, left, right);
			if (enclose == enclose_t::None)
				buff.print<enclose_t::None>(maxdig);
			else if (enclose == enclose_t::Top)
				buff.print<enclose_t::Top>(maxdig);
			else if (enclose == enclose_t::Bottom)
				buff.print<enclose_t::Bottom>(maxdig);
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
