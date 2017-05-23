#pragma once

#include <iostream>
#include "../utility/file_handle.h"
#include "../lexing/position.h"
#include "../lexing/token.h"

namespace yk {
	namespace rep {
		// Message
		class msg {
		public:
			file_handle const& File;
			ystr Message;
			yopt<ystr>	Note;

		public:
			msg(file_handle const& f);

		public:
			msg& message(ystr const& m);
			msg& note(ystr const& n);
			void stamp(std::ostream& os, ysize r, ysize c);
			virtual void print_head(std::ostream& os) const = 0;
			virtual void print_verb(std::ostream& os) const = 0;
		};

		// Warning

		class warn : public msg {
		public:
			warn(file_handle const& f);
		};

		// Error

		class err : public msg {
		public:
			bool		Fatal;

		public:
			err(file_handle const& f, bool ft);
		};

		// Lexical

		class unexpected_eof : public err {
		public:
			position	Position;

		public:
			unexpected_eof(file_handle const& f, position const& p);

		public:
			virtual void print_head(std::ostream& os) const override;
			virtual void print_verb(std::ostream& os) const override;
		};

		class unrecognized_char : public err {
		private:
			position	Position;
			char		Char;

		public:
			unrecognized_char(file_handle const& f, position const& p, char c);

		public:
			virtual void print_head(std::ostream& os) const override;
			virtual void print_verb(std::ostream& os) const override;
		};

		// Syntax

		class expect_token : public err {
		private:
			interval Position;

		public:
			expect_token(file_handle const& f, interval const& p, ystr const& ex, ystr const& go, ystr const& no = "");

		public:
			void set(token const& got, token const& exp);
			virtual void print_head(std::ostream& os) const override;
			virtual void print_verb(std::ostream& os) const override;
		};

		// Semantic

		class type_mismatch : public err {
		private:
			file_handle const& m_File;
			interval m_Position1;
			interval m_Position2;
			ystr m_Message;
		
		public:
			type_mismatch(file_handle const& f, interval const& p1, interval const& p2, ystr const& n = "");

		public:
			virtual void print(std::ostream& os) const override;
		};

		class no_such_symbol : public err {
		private:
			file_handle const& m_File;
			ystr m_Identifier;
			interval m_Position;
			ystr m_HintNote;
			interval m_HintPosition;

		public:
			no_such_symbol(file_handle const& f, ystr const& id, interval const& p, ystr const& hn = "", interval const& hp = interval());

		public:
			virtual void print(std::ostream& os) const override;
		};
	}
}
