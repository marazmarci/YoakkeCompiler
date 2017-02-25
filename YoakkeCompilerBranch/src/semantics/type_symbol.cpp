#include "type_symbol.h"

namespace yk {
	type_symbol::type_symbol(ystr const& id)
		: symbol(id) {
	}

	type_symbol::~type_symbol() { }

	builtin_type_symbol::builtin_type_symbol(ystr const& id)
		: type_symbol(id) {
	}
	
	builtin_type_symbol::~builtin_type_symbol() { }
}