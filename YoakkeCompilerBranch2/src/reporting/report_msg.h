#pragma once

#include <iostream>
#include "../lexing/position.h"
#include "../utility/file_handle.h"

namespace yk {
	namespace rep {
		class msg {
		public:
			virtual ~msg();

		public:
			virtual void print(std::ostream& os) const = 0;
		};

		class err : public msg { };
		class warn : public msg { };

		class lex_err : public err { };
		class parse_err : public err { };
		class semantic_err : public err { };

		class unexpected_eof : public lex_err {
		private:
			file_handle const& m_File;
			position m_Pos;
			ystr m_Note;

		public:
			unexpected_eof(file_handle const& f, position const& p, ystr const& n);
			virtual ~unexpected_eof();

		public:
			virtual void print(std::ostream& os) const override;
		};

		class unrecognized_char : public lex_err {
		private:
			file_handle const& m_File;
			position m_Pos;
			char m_Char;

		public:
			unrecognized_char(file_handle const& f, position const& p, char ch);
			virtual ~unrecognized_char();

		public:
			virtual void print(std::ostream& os) const override;
		};

		class expect_tok : public parse_err {
		private:
			file_handle const& m_File;
			interval m_Pos;
			ystr m_What;
			ystr m_Got;
			ystr m_Note;

		public:
			expect_tok(file_handle const& f, interval const& p, ystr const& w, ystr const& g, ystr const& n);
			virtual ~expect_tok();

		public:
			virtual void print(std::ostream& os) const override;
		};

		class undef_symbol : public semantic_err {

		};
	}
}
