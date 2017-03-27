#include "parse_error.h"
#include "../utility/fmt_in.h"
#include "../utility/fmt_out.h"

namespace yk {
	// Expect error

	expect_parse_err::expect_parse_err(ystr const& wh, ystr const& g, ystr const& file, interval const& pos) 
		: m_What(wh), m_Got(g), m_File(file), m_Position(pos) {
	}

	void expect_parse_err::print(std::ostream& os) const {
		position const& beg = m_Position.Start;
		position const& end = m_Position.End;
		os << m_What << " expected, got: " << m_Got << " in file: '" << m_File
			<< "', at line: " << beg.Row + 1 << ", char: " << beg.Col + 1 
			<< std::endl;
		std::ifstream is(m_File);
		if (is.good()) {
			std::size_t len = end.Col - beg.Col;
			if (len == 0) {
				len++;
			}
			ystr line;
			line_at(is, beg.Row) >> line;
			os	<< line << std::endl
				<< skip(beg.Col) << repeat(len, '^');
		}
		else {
			os << "<could not sample error source>";
		}
	}
}
