#pragma once

#include "lexer.h"
#include "../common.h"
#include "position.h"
#include "ytoken_t.h"

namespace yk {
	class ylexer : public lexer {
	private:
		ystr m_File;
		ystr m_Src;
		const char* m_Ptr;
		position m_Position;
		ymap<ystr, ytoken_t> m_Symbols;
		yopt_map<ystr, ytoken_t> m_Keywords;

	public:
		ylexer(ystr const& file);

	public:
		lexer_state get_state() override;
		void set_state(lexer_state pos) override;

		token next() override;
		bool has_next() override;

		void add_symbol(ystr const& val, ytoken_t tt);
		void add_keyword(ystr const& val, ytoken_t tt);

	private:
		void advance(ysize cnt = 1);
	};
}
