#pragma once

#include "../syntax/token.h"
#include "../common.h"

struct type;

namespace oper_desc {
	bool good_def(token_t op, type* def_arg_t);
}
