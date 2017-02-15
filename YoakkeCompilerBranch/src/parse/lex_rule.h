#pragma once

#include "../common.h"

namespace yk {
	class lex_rule {
	protected:
		lex_rule* m_Next;

	public:
		lex_rule();
		virtual ~lex_rule();

	public:
		void set_next(lex_rule* nxt);
		virtual ysize match(const char* input) const = 0;
	};

	class or_lex_rule : public lex_rule {
	private:
		lex_rule* m_Left;
		lex_rule* m_Right;

	public:
		or_lex_rule(lex_rule* l, lex_rule* r);
		virtual ~or_lex_rule();

	public:
		virtual ysize match(const char* input) const override;
	};

	class seq_lex_rule : public lex_rule {
	private:
		ystr m_Sequence;

	public:
		seq_lex_rule(ystr const& seq);
		virtual ~seq_lex_rule();

	public:
		virtual ysize match(const char* input) const override;
	};

	class mul_lex_rule : public lex_rule {
	private:
		lex_rule* m_Any;

	public:
		mul_lex_rule(lex_rule* any);
		virtual ~mul_lex_rule();

	public:
		virtual ysize match(const char* input) const override;
	};

	class char_lex_rule : public lex_rule {
	private:
		yset<char> m_Chars;

	public:
		char_lex_rule(ystr const& chars);
		char_lex_rule(std::initializer_list<char> list);
		virtual ~char_lex_rule();

	public:
		virtual ysize match(const char* input) const override;
	};

	class chrange_lex_rule : public lex_rule {
	private:
		char m_First;
		char m_Last;

	public:
		chrange_lex_rule(char f, char l);
		virtual ~chrange_lex_rule();

	public:
		virtual ysize match(const char* input) const override;
	};

	namespace lr {
		lex_rule* match(ystr const& str);
		lex_rule* mul(lex_rule* r);
		lex_rule* set(ystr const& chars);
		lex_rule* range(char l, char r);
		lex_rule* or(std::initializer_list<lex_rule*> elems);
	}
}
