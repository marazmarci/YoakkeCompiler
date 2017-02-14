#pragma once

#include "../common.h"

namespace yk {
	class lex_rule {
	public:
		virtual ~lex_rule();

	public:
		virtual ysize match(const char* input) const = 0;
	};

	class and_lex_rule : public lex_rule {
	private:
		lex_rule* m_Left;
		lex_rule* m_Right;

	public:
		and_lex_rule(lex_rule* l, lex_rule* r);
		virtual ~and_lex_rule();

	public:
		virtual ysize match(const char* input) const override;
	};
}