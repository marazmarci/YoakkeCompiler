#pragma once

#include <iostream>
#include "../common.h"

namespace yk {
	class expr;

	class dbg_printer {
	public:
		dbg_printer() = delete;

	public:
		static void print(std::ostream& os, ysptr<expr> const& expr);
	};
}
