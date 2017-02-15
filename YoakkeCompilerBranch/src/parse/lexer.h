#pragma once

#include "token.h"
#include "lex_rule.h"
#include "../utility/iterator.h"
#include "../common.h"

namespace yk {
	class lexer : public iterator<token> {
	private:
		yvec<lex_rule*> m_Skips;
		yvec<ypair<lex_rule*, ystr>> m_Rules;
		const char* m_Src;

	public:
		void set_source(const char* src);

		void add_skip(lex_rule* rule);
		void add_rule(lex_rule* rule, ystr const& tok);

		token next() override;
		bool has_next() override;

	private:
		void skip(ysize delta);
	};
}