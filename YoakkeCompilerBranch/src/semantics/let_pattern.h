#pragma once

#include "../common.h"

namespace yk {
	class type_symbol;
	class pat_expr;
	class expr;

	namespace let_pat {
		yvec<ypair<ystr, expr*>> define(pat_expr* left, expr* exp);
	}
}