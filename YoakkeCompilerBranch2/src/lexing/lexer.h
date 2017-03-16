#pragma once

#include "../common.h"

namespace yk {
	class token;
	class position;

	using lexer_state = ypair<const char*, position>;

	class lexer {
	public:
		virtual lexer_state get_state() = 0;
		virtual void set_state(lexer_state pos) = 0;

		virtual token next() = 0;
		virtual bool has_next() = 0;
	};
}
