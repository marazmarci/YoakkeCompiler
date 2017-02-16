#pragma once

#include "prefix_parselet.h"
#include "infix_parselet.h"

namespace yk {
	class expr;
	class expr_parser;

	typedef prefix_parselet<expr, expr_parser> expr_prefix_parselet;
	typedef infix_parselet<expr, expr_parser> expr_infix_parselet;
}