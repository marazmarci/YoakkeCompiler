#pragma once

#include "combinator.h"

struct label;

namespace parser {
	using namespace combinator;

	result_t<token> p_eof(token_input& in);
	result_t<label> p_label(token_input& in);
}
