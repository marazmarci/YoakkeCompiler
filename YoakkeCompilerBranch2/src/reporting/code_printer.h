#pragma once

#include <iostream>
#include "../common.h"
#include "../utility/file_handle.h"
#include "../lexing/position.h"

namespace yk {
	namespace rep {
		struct mark_buffer {
		private:
			ystr m_Top;
			ystr m_Bottom;

		public:
			ypair<ysize, ysize> set(const char* str, ysize str_len, ysize x, ysize y);
			void point_at(ysize x, ysize y);
			void print(ysize line, ysize maxdigit, ysize buff_width);
		};

		struct code_printer {
		private:
			friend class mark_buffer;

		private:
			static interval				s_ErrorPos;
			static ysize				s_BufferWidth;
			static ysize				s_TabSize;
			static ysize				s_LinesBefore;
			static ysize				s_LinesAfter;
			static file_handle const*	s_File;
			static std::ostream*		s_Ostream;

		public:
			static void print(file_handle const& file, interval const& pos);

		private:
			static void print_marked(ysize from, ysize to, ysize left, ysize right);
			
			static ysize first_printed(ysize ln);
			static ysize last_printed(ysize ln);
		};
	}
}
