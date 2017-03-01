#include "typed_symbol.h"

namespace yk {
	typed_symbol::typed_symbol(ystr const& id, type_symbol* ts)
		: symbol(id), Type(ts), Value(nullptr) {
	}

	typed_symbol::~typed_symbol() { }

	const_bind_symbol::const_bind_symbol(ystr const& id, type_symbol* ts, expr* exp)
		: typed_symbol(id, ts) {
	}

	const_bind_symbol::~const_bind_symbol() { }

	var_symbol::var_symbol(ystr const& id, type_symbol* ts)
		: typed_symbol(id, ts) {
	}

	var_symbol::~var_symbol() { }
}
