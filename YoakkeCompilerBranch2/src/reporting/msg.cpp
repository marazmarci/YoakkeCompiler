#include "msg.h"
#include "code_printer.h"

namespace yk {
	namespace rep {
		// Message

		msg::msg(file_handle const& f)
			: File(f), Message{}, Note{} {
		}

		void msg::stamp(std::ostream& os, ysize r, ysize c) const {
			os << "in file: " << File.name()
				<< "', at line " << r
				<< ", character " << c;
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
		unexpected_eof::unexpected_eof(file_handle const& f, point const& p)
			: err(f, false), Position(p) {
		}

		void unexpected_eof::print_head(std::ostream& os) const {
			os << "Unexpected end of file ";
			stamp(os, Position.Row, Position.Col);
		}

		void unexpected_eof::print_verb(std::ostream& os) const {
			code_printer::print(File, interval(Position, 1));
		}

		// Unknown token
		unrecognized_char::unrecognized_char(file_handle const& f, point const& p, char c)
			: err(f, false), Position(p), Char(c) {
		}

		void unrecognized_char::print_head(std::ostream& os) const {
			os << "Unrecognized character: '" << Char
				<< "' (code: " << int(Char) << ") ";
			stamp(os, Position.Row, Position.Col);
		}

		void unrecognized_char::print_verb(std::ostream& os) const {
			code_printer::print(File, interval(Position, 1));
		}

		// Expect token
		expect_token::expect_token(file_handle const& f, interval const& p)
			: err(f, true), Position(p) {
		}

		expect_token& expect_token::set(ystr const& exp, token const& got) {
			message("Expected: " + exp + ", but got: " + got.format());
			return *this;
		}

		void expect_token::print_head(std::ostream& os) const {
			os << "Syntax error ";
			stamp(os, Position.Start.Row, Position.Start.Col);
		}

		void expect_token::print_verb(std::ostream& os) const {
			code_printer::print(File, Position);
		}

		// Type mismatch
		type_mismatch::type_mismatch(file_handle const& f, interval const& p1, interval const& p2)
			: err(f, true), Position1(p1), Position2(p2) {
		}

		void type_mismatch::print_head(std::ostream& os) const {
			os << "Type mismatch ";
			stamp(os, Position2.Start.Row, Position2.Start.Col);
		}

		void type_mismatch::print_verb(std::ostream& os) const {
			code_printer::print(File, Position1, Position2);
		}

		// Undefined symbol
		undefined_symbol::undefined_symbol(file_handle const& f, ystr const& id, interval const& p, yopt<interval> hp)
			: err(f, true), Identifier(id), Position(p), HintPosition(hp) {
		}

		void undefined_symbol::print_head(std::ostream& os) const {
			os << "Undefined symbol: '" << Identifier << "' ";
			stamp(os, Position.Start.Row, Position.Start.Col);
		}

		void undefined_symbol::print_verb(std::ostream& os) const {
			if (HintPosition) {
				code_printer::print(File, *HintPosition, Position);
			}
			else {
				code_printer::print(File, Position);
			}
		}

		// Unannotated return
		unannot_ret::unannot_ret(file_handle const& f, interval const& p) 
			: err(f, true), Position(p) {
		}

		void unannot_ret::print_head(std::ostream& os) const {
			os << "Unannotated return statement ";
			stamp(os, Position.Start.Row, Position.Start.Col);
		}

		void unannot_ret::print_verb(std::ostream& os) const {
			code_printer::print(File, Position);
		}

		// No return
		no_ret::no_ret(file_handle const& f, interval const& p)
			: err(f, true), Position(p) {
		}

		void no_ret::print_head(std::ostream& os) const {
			os << "No return statement at the end of function ";
			stamp(os, Position.Start.Row, Position.Start.Col);
		}

		void no_ret::print_verb(std::ostream& os) const {
			code_printer::print(File, Position);
		}
	}
}
