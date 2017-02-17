#pragma once

#include "prec_parser.h"

namespace yk {
	class expr;

	class yexpr_parser : public prec_parser<expr> {
	public:
		yexpr_parser(token_buffer* buff);
	};
}