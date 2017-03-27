#include <fstream>
#include "lex_error.h"
#include "../utility/fmt_out.h"
#include "../utility/fmt_in.h"

namespace yk {
	// Unknown token

	unknown_tok_err::unknown_tok_err(ystr const& tok, ystr const& file, position const& pos)
		: m_Token(tok), m_File(file), m_Position(pos) {
	}

	void unknown_tok_err::print(std::ostream& os) const {
		os << "Unknown token: " << m_Token << " in file: '" << m_File
			<< "', at line: " << m_Position.Row + 1 
			<< ", char: " << m_Position.Col + 1
			<< std::endl;
		std::ifstream is(m_File);
		if (is.good()) {
			ystr line;
			line_at(is, m_Position.Row) >> line;
			os << line << std::endl
				<< skip(m_Position.Col) << '^';
		}
		else {
			os << "<could not sample error source>";
		}
	}

	// Could not open file

	lex_no_file_err::lex_no_file_err(ystr const& file)
		: m_File(file) {
	}

	void lex_no_file_err::print(std::ostream& os) const {
		os << "Could not open source file: '" << m_File << "'";
	}

	// Unexpected EOF

	unexpected_eof::unexpected_eof(ystr const& file, position const& pos, ystr const& hint)
		: m_File(file), m_Position(pos), m_Hint(hint) {
	}

	void unexpected_eof::print(std::ostream& os) const {
		os << "Unexpected end of file in file: '" << m_File
			<< "', at line: " << m_Position.Row + 1
			<< ", char: " << m_Position.Col + 1
			<< std::endl;
		std::ifstream is(m_File);
		if (is.good()) {
			ystr line;
			line_at(is, m_Position.Row) >> line;
			os << line << std::endl
				<< skip(m_Position.Col) << '^';
		}
		else {
			os << "<could not sample error source>";
		}
		if (m_Hint.size()) {
			os << std::endl << '(' << m_Hint << ')';
		}
	}
}
