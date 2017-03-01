#include "typed_symbol.h"

namespace yk {
	typed_symbol::typed_symbol(ystr const& id, type_symbol* ts)
		: symbol(id), Type(ts), Value(nullptr) {
	}

	typed_symbol::~typed_symbol() { }

	const_bind_symbol::const_bind_symbol(ystr const& id, type_symbol* ts)
		: typed_symbol(id, ts) {
	}

	const_bind_symbol::~const_bind_symbol() { }

	builtin_function_symbol::builtin_function_symbol(ystr const& id, type_symbol* ts)
		: const_bind_symbol(id, ts) {
	}

	builtin_function_symbol::~builtin_function_symbol() { }

	var_symbol::var_symbol(ystr const& id, type_symbol* ts)
		: typed_symbol(id, ts) {
	}

	var_symbol::~var_symbol() { }
}
