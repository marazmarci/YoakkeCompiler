#pragma once

#include "../common.h"

namespace yk {
	class type_symbol;
	class pattern;
	class expr;

	namespace let_pat {
		yvec<ypair<ystr, expr*>> define(pattern* left, expr* exp);
	}
}