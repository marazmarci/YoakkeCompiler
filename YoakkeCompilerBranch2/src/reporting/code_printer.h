#pragma once

#include <iostream>
#include "../common.h"
#include "../utility/file_handle.h"
#include "../lexing/position.h"

namespace yk {
	namespace rep {
		// TODO: separate internals from console

		struct code_printer {
		private:
			static ysize	s_LinesBefore;
			static ysize	s_LinesAfter;
			static bool		s_LineNumbering;
			static bool		s_LeadingZeroes;
			static ystr		s_LineSeparator;
			static ystr		s_IntSeparator;
			static char		s_ArrowLine;
			static char		s_Arrow;
			static bool		s_IntervalMode;
			static ysize	s_TabSize;
			static ysize	s_MaxInterval;

		private:
			std::ostream&		m_Ostream;
			ysize				m_Width;
			file_handle const*	m_File;
			ysize				m_DigitCount;
			ysize				m_Padding;

		public:
			code_printer(std::ostream& os, ysize w);

		public:
			void print(file_handle const& file, interval const& pos);

		private:
			void print_marked(ysize first, ysize last, ysize left, ysize right);
			void print_line_marked(ysize ln_idx, ysize left, ysize arr);
			void print_line(ysize ln_idx);
			void print_line_padding(ysize num, bool numbering);
			ysize print_part(const char* src, ysize len);

			ysize first_printed(ysize ln);
			ysize last_printed(ysize ln);
		};
	}
}
