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
			static char		s_ArrowLine;
			static char		s_Arrow;
			static bool		s_IntervalMode;

		private:
			std::ostream&		m_Ostream;
			file_handle const*	m_File;
			interval			m_Position;

		public:
			code_printer(std::ostream& os);

		public:
			void print(file_handle const& file, interval const& pos);

		private:
			ysize first_printed(ysize ln);
			ysize last_printed(ysize ln);
		};
	}
}
