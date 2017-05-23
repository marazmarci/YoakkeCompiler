#include "msg.h"
#include "code_printer.h"

namespace yk {
	namespace rep {
		// Message

		msg::msg(file_handle const& f)
			: File(f), Note{} {
		}

		void msg::stamp(std::ostream& os, ysize r, ysize c) {
			os << "in file: " << File.name()
				<< "', at line: " << r
				<< ", character: " << c;
		}

		msg& msg::message(ystr const& m) {
			Message = m;
			return *this;
		}

		msg& msg::note(ystr const& n) {
			Note = n;
			return *this;
		}

		// Warning

		warn::warn(file_handle const& f)
			: msg(f) {
		}

		// Error

		err::err(file_handle const& f, bool ft)
			: msg(f), Fatal(ft) {
		}

		// Lexical

		// EOF
		unexpected_eof::unexpected_eof(file_handle const& f, position const& p)
			: err(f, false), Position(p) {
		}

		void unexpected_eof::print_head(std::ostream& os) const {
			os << "Unexpected end of file";
		}

		void unexpected_eof::print_verb(std::ostream& os) const {
			code_printer::print(File, interval(Position, 1));
		}

		// Unknown token
		unrecognized_char::unrecognized_char(file_handle const& f, position const& p, char c)
			: err(f, false), Position(p), Char(c) {
		}

		void unrecognized_char::print_head(std::ostream& os) const {
			os << "Unrecognized character: '" << Char << "' (code: " << int(Char) << ")";
		}

		void unrecognized_char::print_verb(std::ostream& os) const {
			code_printer::print(File, interval(Position, 1));
		}

		// Expect token
		void expect_token::set(token const& got, token const& exp) {

		}

		void expect_token::print_head(std::ostream& os) const {
			os << "Syntax error";
		}

		void expect_token::print_verb(std::ostream& os) const {
		
		}
	}
}
