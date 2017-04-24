#pragma once

#include <iostream>
#include "../common.h"
#include "../utility/file_handle.h"
#include "../lexing/position.h"

namespace yk {
	namespace rep {
		struct code_printer {
		private:
			static ysize	s_LinesBefore;
			static ysize	s_LinesAfter;
			static bool		s_LineNumbering;
			static bool		s_LeadingZeroes;
			static ystr		s_LineSeparator;
			static char		s_ArrowLine;
			static char		s_Arrow;
			static bool		s_ArrowAbove;
			static bool		s_IntervalMode;

		public:
			static void print(std::ostream& os, file_handle const& file, interval const& pos);

		private:
			static void print_line(std::ostream& os, file_handle const& file, ysize ln, ysize max_num_len, ysize pad);
			static void print_line_begin(std::ostream& os, ysize ln, ysize max_num_len, bool first);

		public:
			code_printer() = delete;
		};
	}
}
