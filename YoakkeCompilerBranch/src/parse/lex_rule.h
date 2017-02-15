#pragma once

#include "../common.h"

namespace yk {
	namespace lex_rule {
		enum class code {
			end, chars, mul, opt
		};

		struct rule {
		public:
			rule* Next;

		protected:
			code m_Hash;

		protected:
			rule(code c);

		public:
			virtual ~rule();

		public:
			code hash_code() const;

		public:
			friend rule& operator+(rule& l, rule& r);
		};

		struct end : public rule {
		public:
			end();
			virtual ~end();
		};

		struct chars : public rule {
		public:
			ystr Sequence;

		public:
			chars(ystr const& seq);
			virtual ~chars();
		};

		struct mul : public rule {
		public:
			rule* Sub;

		public:
			mul(rule& sub);
			virtual ~mul();
		};

		struct opt : public rule {
		public:
			rule* Sub;

		public:
			opt(rule& sub);
			virtual ~opt();
		};

		extern end END;

		ysize match(const char* str, rule& thebegin);
	}
}
