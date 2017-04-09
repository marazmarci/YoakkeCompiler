#include "type_symbol.h"

namespace yk {
	type_symbol::type_symbol(ystr const& id)
		: symbol(id) {
	}

	type_symbol::~type_symbol() { }

	// Native type symbol

	native_type_symbol::native_type_symbol(ystr const& id)
		: type_symbol(id) {
	}

	native_type_symbol::~native_type_symbol() { }
}
