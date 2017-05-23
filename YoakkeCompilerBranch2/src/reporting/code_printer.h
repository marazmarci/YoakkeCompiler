#pragma once

#include <algorithm>
#include <iostream>
#include "../common.h"
#include "../utility/file_handle.h"
#include "../lexing/position.h"
#include "../utility/fmt_out.h"
#include "../utility/math.h"

namespace yk {
	namespace rep {
		template <bool DRAW_ARROW, bool IN_INTERVAL>
		struct mark_buffer;

		enum class enclose_t {
			None, Top, Bottom
		};

		struct code_printer {
		public:
			static ysize				s_BufferWidth;
			static ysize				s_TabSize;
			static ysize				s_LinesBefore;
			static ysize				s_LinesAfter;
			static ysize				s_MaxInterval;
			static bool					s_LineNumbering;
			static ystr					s_NumberSep;
			static ystr					s_IntervalSep;
			static char					s_LineBase;
			static char					s_LineHead;
			static file_handle const*	s_File;
			static std::ostream*		s_Ostream;

		public:
			static void initialize();
			static void print(file_handle const& file, interval const& pos);
			static void print(file_handle const& file, interval const& pos1, interval const& pos2);

		private:
			static void print_marked(ysize from, ysize to, ysize left, ysize right);
			static void print_marked(ysize from, ysize to, ysize s1, ysize e1, ysize s2, ysize e2);
			
			template <enclose_t ENCLOSE>
			static void print_marked_single(ysize ln_idx, ysize maxdig, ysize left, ysize right);

			template <bool IN_INTERVAL>
			static void print_unmarked_single(ysize ln_idx, ysize maxdig) {
				mark_buffer<false, IN_INTERVAL> buff;
				buff.set(*s_File, ln_idx);
				buff.print(maxdig);
			}

			static ysize first_printed(ysize ln);
			static ysize last_printed(ysize ln);
		};

		struct mark_buffer_base {
		protected:
			ystr m_Buffer;
			ysize m_LineIndex;
			ysize m_LineDigitCnt;

		protected:
			bool consume(char c) {
				if (c == '\t') {
					for (ysize j = 0; j < code_printer::s_TabSize; j++) {
						m_Buffer += ' ';
					}
					return true;
				}
				else {
					m_Buffer += c;
					return false;
				}
			}

			void print_padding(bool first, ysize max_dig, bool in_int) {
				std::ostream& os = *code_printer::s_Ostream;

				if (code_printer::s_LineNumbering) {
					if (first) {
						os << fmt::repeat(max_dig - m_LineDigitCnt, '0')
							<< m_LineIndex << code_printer::s_NumberSep;
					}
					else {
						os << fmt::skip(max_dig) << code_printer::s_NumberSep;
					}
				}
				if (in_int) {
					os << code_printer::s_IntervalSep;
				}
			}

			ysize print_part(ysize offs, ysize buff_width) {
				std::ostream& os = *code_printer::s_Ostream;

				ysize written = std::min(offs + buff_width, m_Buffer.length());
				for (ysize i = offs; i < written; i++) {
					os << m_Buffer[i];
				}
				os << std::endl;
				return written;
			}

			ysize calc_padding(ysize maxdigit) {
				if (code_printer::s_LineNumbering) {
					return maxdigit + code_printer::s_NumberSep.length();
				}
				else {
					return 0;
				}
			}
		};

		template <bool DRAW_ARROW, bool IN_INTERVAL>
		struct mark_buffer {};

		template <>
		struct mark_buffer<true, false> : public mark_buffer_base {
		private:
			ysize m_Begin;
			ysize m_End;

		public:
			void set(file_handle const& file, ysize line_idx, ysize x, ysize y) {
				m_LineIndex = line_idx;
				m_LineDigitCnt = math::digit_count(line_idx);
				
				ysize newx = x;
				ysize newy = y;

				const char* str = file.line(line_idx);
				ysize str_len = file.line_len(line_idx);

				m_Buffer.reserve(str_len);

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

				m_Begin = newx;
				m_End = newy;
			}

			ysize print_arrow_beg(ysize padding, ysize offs, ysize buff_width, bool extra) {
				std::ostream& os = *code_printer::s_Ostream;

				ysize to_write = std::min(m_End - m_Begin, buff_width - (m_Begin - offs));
				if (extra) {
					os
						<< fmt::skip(padding)
						<< fmt::repeat(m_Begin - offs + code_printer::s_IntervalSep.length(), code_printer::s_LineBase)
						<< fmt::repeat(to_write, code_printer::s_LineHead)
						<< std::endl;
				}
				else {
					os
						<< fmt::skip(padding)
						<< fmt::repeat(m_Begin - offs, code_printer::s_LineBase)
						<< fmt::repeat(to_write, code_printer::s_LineHead)
						<< std::endl;
				}
				return to_write;
			}

			ysize print_arrow_head(ysize padding, ysize written, ysize buff_width) {
				std::ostream& os = *code_printer::s_Ostream;

				ysize to_write = std::min(m_End - written, buff_width);
				os
					<< fmt::skip(padding)
					<< fmt::repeat(to_write, code_printer::s_LineHead)
					<< std::endl;
				return to_write;
			}

			template <enclose_t ENCLOSE>
			void print(ysize maxdigit) {
			}

			template <>
			void print<enclose_t::None>(ysize maxdigit) {
				std::ostream& os = *code_printer::s_Ostream;

				ysize padding = calc_padding(maxdigit);
				ysize buff_width = code_printer::s_BufferWidth - padding;
				ysize arr_written = m_Begin;
				bool started = false;

				ysize written = 0;
				ysize len = m_Buffer.length();
				if (len == 0) {
					print_padding(true, maxdigit, false);
					os << std::endl;
				}
				for (ysize offs = 0; offs < len; offs = written) {
					// First needs number, the rest blank
					print_padding(offs == 0, maxdigit, false);

					written = print_part(offs, buff_width);

					if (!started) {
						if (written >= m_Begin) {
							started = true;
							arr_written += print_arrow_beg(padding, offs, buff_width, false);
						}
					}
					else if (arr_written < m_End) {
						arr_written += print_arrow_head(padding, arr_written, buff_width);
					}
				}
			}

			template <>
			void print<enclose_t::Top>(ysize maxdigit) {
				std::ostream& os = *code_printer::s_Ostream;

				ysize padding = calc_padding(maxdigit);
				ysize buff_width = code_printer::s_BufferWidth - padding;

				ysize arr_written = m_Begin;

				bool in_interval = false;
				bool started = false;

				ysize written = 0;
				ysize len = m_Buffer.length();
				if (len == 0) {
					print_padding(true, maxdigit, in_interval);
					os << std::endl;
				}
				for (ysize offs = 0; offs < len; offs = written) {
					// First needs number, the rest blank
					print_padding(offs == 0, maxdigit, in_interval);

					written = print_part(offs, buff_width);

					if (!started) {
						if (!in_interval && written >= m_Begin) {
							in_interval = true;
							started = true;
							arr_written += print_arrow_beg(padding, offs, buff_width, false);

							padding += code_printer::s_IntervalSep.length();
							buff_width -= code_printer::s_IntervalSep.length();
						}
					}
					else if (arr_written < m_End) {
						arr_written += print_arrow_head(padding, arr_written, buff_width);
					}
				}
			}

			template <>
			void print<enclose_t::Bottom>(ysize maxdigit) {
				std::ostream& os = *code_printer::s_Ostream;

				ysize padding = calc_padding(maxdigit);
				ysize buff_width = code_printer::s_BufferWidth - padding;

				ysize arr_written = m_Begin;
				
				bool in_interval = true;
				bool started = false;

				padding += code_printer::s_IntervalSep.length();
				buff_width -= code_printer::s_IntervalSep.length();

				ysize written = 0;
				ysize len = m_Buffer.length();
				if (len == 0) {
					print_padding(true, maxdigit, in_interval);
					os << std::endl;
				}
				for (ysize offs = 0; offs < len; offs = written) {
					// First needs number, the rest blank
					print_padding(offs == 0, maxdigit, in_interval);

					written = print_part(offs, buff_width);

					if (!started) {
						if (in_interval && written >= m_Begin) {
							in_interval = false;
							started = true;
							padding -= code_printer::s_IntervalSep.length();
							buff_width += code_printer::s_IntervalSep.length();
							arr_written += print_arrow_beg(padding, offs, buff_width, true);
						}
					}
					else if (arr_written < m_End) {
						arr_written += print_arrow_head(padding, arr_written, buff_width);
					}
				}
			}
		};

		template <bool IN_INTERVAL>
		struct mark_buffer<false, IN_INTERVAL> : mark_buffer_base {
		public:
			void set(file_handle const& file, ysize line_idx) {
				m_LineIndex = line_idx;
				m_LineDigitCnt = math::digit_count(line_idx);

				const char* str = file.line(line_idx);
				ysize str_len = file.line_len(line_idx);

				m_Buffer.reserve(str_len);
				for (ysize i = 0; i < str_len; i++) {
					consume(str[i]);
				}
			}

			void print(ysize maxdigit) {
				std::ostream& os = *code_printer::s_Ostream;

				ysize padding = calc_padding(maxdigit);
				ysize buff_width = code_printer::s_BufferWidth - padding;

				ysize written = 0;
				ysize len = m_Buffer.length();
				if (len == 0) {
					print_padding(true, maxdigit, IN_INTERVAL);
					os << std::endl;
				}
				for (ysize offs = 0; offs < len; offs = written) {
					print_padding(offs == 0, maxdigit, IN_INTERVAL);

					written = print_part(offs, buff_width);
				}
			}
		};

		// IMPLEMENTATION /////////////////////////////////////////////////

		template <enclose_t ENCLOSE>
		void code_printer::print_marked_single(ysize ln_idx, ysize maxdig, ysize left, ysize right) {
			mark_buffer<true, false> buff;
			buff.set(*s_File, ln_idx, left, right);
			buff.print<ENCLOSE>(maxdig);
		}
	}
}
