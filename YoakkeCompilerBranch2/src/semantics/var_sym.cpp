#include "var_sym.h"

namespace yk {
	var_sym::var_sym(ystr const& n, ysptr<type> t)
		: Name(n), Type(t) {
	}

	variable::variable(ystr const& n, ysptr<type> t)
		: var_sym(n, t) {
	}

	constant::constant(ystr const& n, ysptr<type> t)
		: var_sym(n, t) {
	}
}
