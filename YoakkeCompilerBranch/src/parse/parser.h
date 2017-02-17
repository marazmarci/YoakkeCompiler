#pragma once

#include "token_buffer.h"

namespace yk {
	class parser {
	private:
		token_buffer* m_Buffer;

	public:
		parser(token_buffer* buff);

	public:
		token const& peek(ysize delta = 0);
		token consume();
		yopt<token> match_val(ystr const& val);
		yopt<token> match_id(ystr const& val);
	};
}