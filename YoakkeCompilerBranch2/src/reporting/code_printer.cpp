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
		bool	code_printer::s_IntervalMode	= true;

		code_printer::code_printer(std::ostream& os)
			: m_Ostream(os) {
		}

		void code_printer::print(file_handle const& file, interval const& pos) {
			m_File		= &file;
			m_Position	= pos;

			// Get bounds
			ysize first_marked	= pos.Start.Row;
			ysize last_marked	= pos.End.Row;
			ysize first_before	= first_printed(first_marked);
			ysize last_after	= last_printed(last_marked);


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
