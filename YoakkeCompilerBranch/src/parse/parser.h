#pragma once

#include "token_buffer.h"

namespace yk {
	struct parse_state {
	public:
		yvec<token> BufferCopy;

	public:
		parse_state(yvec<token> const& buff);
		~parse_state();
	};

	class parser {
	private:
		token_buffer* m_Buffer;
		
	protected:
		parser* m_Parent;

	public:
		parser(token_buffer* buff, parser* par = nullptr);

	public:
		token const& peek(ysize delta = 0);
		token consume();
		yopt<token> match_val(ystr const& val);
		yopt<token> match_id(ystr const& val);

		parse_state get_state();
		void load_state(parse_state const& st);

		parser* parent();
	};
}