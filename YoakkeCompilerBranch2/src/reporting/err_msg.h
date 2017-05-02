#pragma once

#include <iostream>
#include "../utility/file_handle.h"
#include "../lexing/position.h"

namespace yk {
	namespace rep {
		// Generic
		class msg {
		public:
			virtual void print(std::ostream& os) const = 0;
		};

		class err : public msg {
		private:
			bool m_CanContinue;

		public:
			err(bool cont);

		public:
			bool can_continue() const;
		};

		class warn : public msg { };

		// Lexical

		class unexpected_eof : public err {
		private:
			file_handle const& m_File;
			position m_Position;
			ystr m_Message;
			ystr m_Note;

		public:
			unexpected_eof(file_handle const& f, position const& p, ystr const& m = "", ystr const& n = "");

		public:
			virtual void print(std::ostream& os) const override;
		};

		class unrecognized_char : public err {
		private:
			file_handle const& m_File;
			position m_Position;
			char m_Char;

		public:
			unrecognized_char(file_handle const& f, position const& p, char c);

		public:
			virtual void print(std::ostream& os) const override;
		};

		// Syntax

		class expect_token : public err {
		private:
			file_handle const& m_File;
			interval m_Position;
			ystr m_Expected;
			ystr m_Got;
			ystr m_Note;

		public:
			expect_token(file_handle const& f, interval const& p, ystr const& ex, ystr const& go, ystr const& no = "");

		public:
			virtual void print(std::ostream& os) const override;
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
	}
}
