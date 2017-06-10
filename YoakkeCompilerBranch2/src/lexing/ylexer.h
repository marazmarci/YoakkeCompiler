#pragma once

#include "lexer.h"
#include "../common.h"
#include "position.h"
#include "ytoken_t.h"
#include "../utility/file_handle.h"

namespace yk {
	class ylexer : public lexer {
	private:
		file_handle&				m_File;			// Source file
		const char*					m_Ptr;			// Pointer in the source
		point					m_Position;		// Counts lines and columns
		point					m_LastVisible;	// Holds the last non-space position

		ymap<ystr, ytoken_t>		m_Symbols;		// Symbolic tokens
		yopt_map<ystr, ytoken_t>	m_Keywords;		// Reserved keywords

	public:
		ylexer(file_handle& file);

	public:
		// State manipulation
		lexer_state get_state()	const	override;
		void set_state(lexer_state pos)	override;

		// Retrieving tokens
		token next()			override;
		bool has_next() const	override;

	private:
		// Declaration of rules
		void add_symbol(ystr const& val, ytoken_t tt);
		void add_keyword(ystr const& val, ytoken_t tt);

		// Step in the file
		void advance(ysize cnt = 1);
	};
}
