#include "report_msg.h"
#include "../utility/fmt_out.h"

namespace yk {
	namespace rep {
		// Abstract report message

		msg::~msg() { }

		// Unexpected eof

		unexpected_eof::unexpected_eof(file_handle const& f, position const& p, ystr const& n)
			: m_File(f), m_Pos(p), m_Note(n) {
		}

		unexpected_eof::~unexpected_eof() { }

		void unexpected_eof::print(std::ostream& os) const {
			os << "unexpected end-of-file in file '"
				<< m_File.name() << "' at line " << m_Pos.Row
				<< ", character " << m_Pos.Col << '.' << std::endl
				<< fmt::part_until(m_File.line(m_Pos.Row), '\n') << std::endl
				<< fmt::skip(m_Pos.Col) << '^' << std::endl
				<< m_Note;
		}

		// Unrecognized character

		unrecognized_char::unrecognized_char(file_handle const& f, position const& p, char ch)
			: m_File(f), m_Pos(p), m_Char(ch) {
		}

		unrecognized_char::~unrecognized_char() { }

		void unrecognized_char::print(std::ostream& os) const {
			os	<< "unrecognized character '" << m_Char
				<< "(code: " << +m_Char << ") " 
				<< "in file: '"
				<< m_File.name() << "' at line " << m_Pos.Row
				<< ", character " << m_Pos.Col << '.' << std::endl
				<< fmt::part_until(m_File.line(m_Pos.Row), '\n') << std::endl
				<< fmt::skip(m_Pos.Col) << '^';
		}

		// Expect a token (or grammar)
		
		expect_tok::expect_tok(file_handle const& f, interval const& p, ystr const& w, ystr const& g, ystr const& n)
			: m_File(f), m_Pos(p), m_What(w), m_Got(g), m_Note(n) {
		}

		expect_tok::~expect_tok() { }

		void expect_tok::print(std::ostream& os) const {
			position const& beg = m_Pos.Start;
			position const& end = m_Pos.End;
			ysize len = end.Col - beg.Col;
			os << m_What << " expected, got " << m_Got << " in file '"
				<< m_File.name() << "', at line " << beg.Row
				<< ", character " << beg.Col << std::endl
				<< fmt::part_until(m_File.line(beg.Row), '\n') << std::endl
				<< fmt::skip(beg.Col) << fmt::repeat(len == 0 ? 1 : len, '^');
			if (m_Note.length()) {
				os << std::endl << m_Note;
			}
		}
	}
}
