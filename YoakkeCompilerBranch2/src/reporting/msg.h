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
			yopt<ystr>	Message;
			yopt<ystr>	Note;

		public:
			msg(file_handle const& f);

		public:
			msg& message(ystr const& m);
			msg& note(ystr const& n);
			void stamp(std::ostream& os, ysize r, ysize c) const;
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
		public:
			interval Position;

		public:
			expect_token(file_handle const& f, interval const& p);

		public:
			expect_token& set(ystr const& exp, token const& got);
			virtual void print_head(std::ostream& os) const override;
			virtual void print_verb(std::ostream& os) const override;
		};

		// Semantic

		class type_mismatch : public err {
		public:
			interval Position1;
			interval Position2;
		
		public:
			type_mismatch(file_handle const& f, interval const& p1, interval const& p2);

		public:
			virtual void print_head(std::ostream& os) const override;
			virtual void print_verb(std::ostream& os) const override;
		};

		class undefined_symbol : public err {
		public:
			ystr			Identifier;
			interval		Position;
			yopt<interval>	HintPosition;

		public:
			undefined_symbol(file_handle const& f, ystr const& id, interval const& p, yopt<interval> hp = {});

		public:
			virtual void print_head(std::ostream& os) const override;
			virtual void print_verb(std::ostream& os) const override;
		};

		class unannot_ret : public err {
		public:
			interval Position;

		public:
			unannot_ret(file_handle const& f, interval const& p);

		public:
			virtual void print_head(std::ostream& os) const override;
			virtual void print_verb(std::ostream& os) const override;
		};

		class no_ret : public err {
		public:
			interval Position;

		public:
			no_ret(file_handle const& f, interval const& p);

		public:
			virtual void print_head(std::ostream& os) const override;
			virtual void print_verb(std::ostream& os) const override;
		};
	}
}
