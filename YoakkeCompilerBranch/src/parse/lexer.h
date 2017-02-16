#pragma once

#include <functional>

#include "token.h"
#include "../utility/iterator.h"
#include "../utility/optional.h"
#include "../common.h"

namespace yk {
	class lexer : public iterator<token> {
	private:
		typedef ysize(*lex_skip)(const char*);
		typedef yopt<ypair<ystr, ysize>>(*lex_rule)(const char*);

	private:
		yvec<lex_skip> m_Skips;
		yvec<lex_rule> m_Rules;
		const char* m_Src;

	public:
		void set_source(const char* src);

		void add_skip(lex_skip rule);
		void add_rule(lex_rule rule);

		token next() override;
		bool has_next() override;

	private:
		void skip(ysize delta);
	};
}