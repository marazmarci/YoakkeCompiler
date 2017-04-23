#include "report_stream.h"

namespace yk {
	namespace rep {
		__endrep endr;

		report_stream& r() {
			return report_stream::instance();
		}

		std::ostream& report_stream::s_Ostream = std::cout;

		report_stream::report_stream() { }

		report_stream& report_stream::instance() {
			static report_stream instance;
			return instance;
		}

		report_stream& report_stream::operator<<(msg const& msg) {
			if (dynamic_cast<lex_err const*>(&msg)) {
				s_Ostream << "Lexical error: ";
			}
			else if (dynamic_cast<parse_err const*>(&msg)) {
				s_Ostream << "Syntax error: ";
			}
			msg.print(s_Ostream);
			return *this;
		}

		report_stream& report_stream::operator<<(rep::__endrep end) {
			s_Ostream << std::endl;
			return *this;
		}
	}
}
