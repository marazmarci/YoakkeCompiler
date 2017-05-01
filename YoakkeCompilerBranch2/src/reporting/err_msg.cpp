#include "err_msg.h"
#include "code_printer.h"

namespace yk {
	namespace rep {
		err::err(bool cont)
			: m_CanContinue(cont) {
		}

		bool err::can_continue() const {
			return m_CanContinue;
		}

		// Unexpected EOF

		unexpected_eof::unexpected_eof(file_handle const& f, position const& p, ystr const& m, ystr const& n)
			: err(true), m_File(f), m_Position(p), m_Message(m), m_Note(n) {
		}

		void unexpected_eof::print(std::ostream& os) const {
			os
				<< "Unexpected end of file in file: '"
				<< m_File.name()
				<< "', at line: " << m_Position.Row
				<< ", character: " << m_Position.Col
				<< ':' << std::endl;
			code_printer::print(m_File, interval(m_Position, 1));
			if (m_Message.length()) {
				os << m_Message << std::endl;
			}
			if (m_Note.length()) {
				os << "Note: " << m_Note << std::endl;
			}
		}

		// Unrecognized char

		unrecognized_char::unrecognized_char(file_handle const& f, position const& p, char c)
			: err(true), m_File(f), m_Position(p), m_Char(c) {
		}

		void unrecognized_char::print(std::ostream& os) const {
			os
				<< "Unrecognized character in file: '"
				<< m_File.name()
				<< "', at line: " << m_Position.Row
				<< ", character: " << m_Position.Col
				<< ':' << std::endl;
			code_printer::print(m_File, interval(m_Position, 1));
			os << "The character is: '" << m_Char
				<< "' (code: " << (int)m_Char << ")." << std::endl;
		}

		// Expect token

		expect_token::expect_token(file_handle const& f, interval const& p, ystr const& ex, ystr const& go, ystr const& no)
			: err(false), m_File(f), m_Position(p), m_Expected(ex), m_Got(go), m_Note(no) {
		}

		void expect_token::print(std::ostream& os) const {
			os
				<< "Syntax error in file: '"
				<< m_File.name()
				<< "', at line: " << m_Position.Start.Row
				<< ", character: " << m_Position.Start.Col
				<< ':' << std::endl;
			code_printer::print(m_File, m_Position);
			os << m_Expected
				<< " expected but got "
				<< m_Got
				<< std::endl;
			if (m_Note.length()) {
				os << m_Note << std::endl;
			}
		}
	}
}
