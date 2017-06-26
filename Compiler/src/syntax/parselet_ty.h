#pragma once

#include "parser.h"

struct AST_ty;

namespace parselet_ty {
	AST_ty* get_ty(parser& p, ysize prec = 0);
}
