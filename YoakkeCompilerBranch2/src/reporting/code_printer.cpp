#include "code_printer.h"
#include "../utility/console.h"

namespace yk {
	namespace rep {
		ysize				code_printer::s_BufferWidth		= 20;
		ysize				code_printer::s_TabSize			= 4;
		ysize				code_printer::s_LinesBefore		= 1;
		ysize				code_printer::s_LinesAfter		= 1;
		ysize				code_printer::s_MaxInterval		= 3;
		bool				code_printer::s_LineNumbering	= true;
		ystr				code_printer::s_NumberSep		= " | ";
		ystr				code_printer::s_IntervalSep		= "| ";
		char				code_printer::s_LineBase		= '~';
		char				code_printer::s_LineHead		= '^';
		std::ostream*		code_printer::s_Ostream			= &std::cout;
		file_handle const*	code_printer::s_File;

		void code_printer::initialize() {
			s_BufferWidth = console::Width;
		}

		void code_printer::print(file_handle const& file, interval const& pos) {
			s_File = &file;
			print_marked(pos.Start.Row, pos.End.Row, pos.Start.Col, pos.End.Col);
		}
		
		void code_printer::print(file_handle const& file, interval const& pos1, interval const& pos2) {
			s_File = &file;
			print_marked(pos1.Start.Row, pos2.End.Row, 
				pos1.Start.Col, pos1.End.Col,
				pos2.Start.Col, pos2.End.Col);
		}

		void code_printer::print_marked(ysize from, ysize to, ysize left, ysize right) {
			ysize first = first_printed(from);
			ysize last = last_printed(to);
			ysize last_dig_cnt = math::digit_count(last);

			for (ysize i = first; i < from; i++) {
				print_unmarked_single<false>(i, last_dig_cnt);
			}

			if (from == to) {
				print_marked_single<enclose_t::None>(from, last_dig_cnt, left, right);
			}
			else {
				ysize dist = to - from;
				print_marked_single<enclose_t::Top>(from, last_dig_cnt, left, left + 1);
				if (dist > s_MaxInterval) {
					print_unmarked_single<true>(from + 1, last_dig_cnt);
					*s_Ostream << fmt::skip(last_dig_cnt + s_NumberSep.length())
						<< "..." << std::endl;
				}
				else {
					for (ysize i = from + 1; i < to; i++) {
						print_unmarked_single<true>(i, last_dig_cnt);
					}
				}
				print_marked_single<enclose_t::Bottom>(to, last_dig_cnt, right - 1, right);
			}

			for (ysize i = to + 1; i <= last; i++) {
				print_unmarked_single<false>(i, last_dig_cnt);
			}
		}

		void code_printer::print_marked(ysize from, ysize to, ysize s1, ysize e1, ysize s2, ysize e2) {
			ysize first = first_printed(from);
			ysize last = last_printed(to);
			ysize last_dig_cnt = math::digit_count(last);

			for (ysize i = first; i < from; i++) {
				print_unmarked_single<false>(i, last_dig_cnt);
			}

			if (from == to) {
				// TODO: double-arrow for single-lined?
				print_marked_single<enclose_t::None>(from, last_dig_cnt, s1, e2);
			}
			else {
				ysize dist = to - from;
				print_marked_single<enclose_t::Top>(from, last_dig_cnt, s1, e1);
				if (dist > s_MaxInterval) {
					print_unmarked_single<true>(from + 1, last_dig_cnt);
					*s_Ostream << fmt::skip(last_dig_cnt + s_NumberSep.length())
						<< "..." << std::endl;
				}
				else {
					for (ysize i = from + 1; i < to; i++) {
						print_unmarked_single<true>(i, last_dig_cnt);
					}
				}
				print_marked_single<enclose_t::Bottom>(to, last_dig_cnt, s2, e2);
			}

			for (ysize i = to + 1; i <= last; i++) {
				print_unmarked_single<false>(i, last_dig_cnt);
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
