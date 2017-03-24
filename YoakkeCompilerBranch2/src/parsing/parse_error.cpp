#include "parse_error.h"
#include "../utility/fmt_in.h"
#include "../utility/fmt_out.h"

namespace yk {
	std::ostream& operator<<(std::ostream& os, parse_err const& err) {
		err.print(os);
		return os;
	}

	// Expect error

	expect_parse_err::expect_parse_err(std::string const& wh, std::string const& g, std::string const& file, interval const& pos) 
		: m_What(wh), m_Got(g), m_File(file), m_Position(pos) {
	}

	void expect_parse_err::print(std::ostream& os) const {
		position const& beg = m_Position.Start;
		position const& end = m_Position.End;
		os << m_What << " expected, got: " << m_Got << " in file: '" << m_File
			<< "', at line: " << beg.Row << ", char: " << beg.Col << std::endl;
		std::ifstream is(m_File);
		if (is.good()) {
			std::string line;
			line_at(is, beg.Row) >> line;
			os << line << std::endl;
			os << skip(beg.Col) <<
				repeat(end.Col - beg.Col, '^') << std::endl;
		}
		else {
			os << "<could not sample error source>" << std::endl;
		}
	}
}
