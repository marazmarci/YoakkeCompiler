#include "symbol.h"

namespace yk {
	symbol symbol::create_constant(ystr const& n, type const& t) {
		return symbol(n, t, std::make_shared<constant_symbol>());
	}

	symbol symbol::create_variable(ystr const& n, type const& t) {
		return symbol(n, t, std::make_shared<variable_symbol>());
	}
}
