#pragma once

#include "../common.h"

namespace yk {
	struct type;

	struct var_sym {
	public:
		ystr		Name;
		ysptr<type> Type;

	public:
		var_sym(ystr const& n, ysptr<type> t);
	};

	struct variable : public var_sym {
	public:
		variable(ystr const& n, ysptr<type> t);
	};

	struct constant : public var_sym {
	public:
		constant(ystr const& n, ysptr<type> t);
	};
}
