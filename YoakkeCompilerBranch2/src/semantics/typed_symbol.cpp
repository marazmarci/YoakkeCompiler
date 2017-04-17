#include "typed_symbol.h"

namespace yk {
	// Abstract typed symbol

	typed_symbol::typed_symbol(ystr const& id, yshared_ptr<type_symbol> ty)
		: symbol(id), Type(ty) {
	}

	typed_symbol::~typed_symbol() { }

	// Constant symbol

	const_typed_symbol::const_typed_symbol(ystr const& id, yshared_ptr<type_symbol> ty)
		: typed_symbol(id, ty) {
	}

	const_typed_symbol::~const_typed_symbol() { }

	// Variable symbol

	var_typed_symbol::var_typed_symbol(ystr const& id, yshared_ptr<type_symbol> ty)
		: typed_symbol(id, ty) {
	}

	var_typed_symbol::~var_typed_symbol() { }
}
