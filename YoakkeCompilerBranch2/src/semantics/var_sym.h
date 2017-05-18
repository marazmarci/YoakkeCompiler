#pragma once

#include "../common.h"

namespace yk {
	class type;

	class var_sym {
	public:
		ystr		Name;
		ysptr<type> Type;

	public:
		var_sym(ystr const& n, ysptr<type> t);
	};

	class variable : public var_sym {
	public:
		variable(ystr const& n, ysptr<type> t);
	};

	class constant : public var_sym {
	public:
		constant(ystr const& n, ysptr<type> t);
	};
}
