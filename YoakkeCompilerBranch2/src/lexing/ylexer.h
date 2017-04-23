#pragma once

#include "lexer.h"
#include "../common.h"
#include "position.h"
#include "ytoken_t.h"
#include "file_handle.h"

namespace yk {
	class ylexer : public lexer {
	private:
		file_handle& m_FileHandle;
		const char* m_Ptr;
		position m_Position;
		position m_LastVisible;
		ymap<ystr, ytoken_t>		m_Symbols;
		yopt_map<ystr, ytoken_t>	m_Keywords;

	public:
		ylexer(file_handle& file);

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
