#pragma once

#include "type.h"
#include "../common.h"

namespace yk {
	struct type;

	class var_sym {
	public:
		ystr		Name;
		type		Type;

	public:
		var_sym(ystr const& n, type const& t);
	};

	class variable : public var_sym {
	public:
		variable(ystr const& n, type const& t);
	};

	class constant : public var_sym {
	public:
		constant(ystr const& n, type const& t);
	};
}
