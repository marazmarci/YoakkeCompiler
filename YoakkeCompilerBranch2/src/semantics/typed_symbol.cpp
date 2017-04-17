#include "typed_symbol.h"

namespace yk {
	// Abstract typed symbol

	typed_symbol::typed_symbol(ystr const& id, yshared_ptr<type_symbol> ty)
		: symbol(id), Type(ty) {
	}

	typed_symbol::~typed_symbol() { }

	// Constant symbol

	ystr const_typed_symbol::create_operator_name(token const& op) {
		return "@operator" + std::to_string(op.Type);
	}

	const_typed_symbol::const_typed_symbol(ystr const& id, yshared_ptr<type_symbol> ty)
		: typed_symbol(id, ty) {
	}

	const_typed_symbol::~const_typed_symbol() { }

	// Builtin symbol (like an operator)

	builtin_typed_symbol::builtin_typed_symbol(ystr const& id, yshared_ptr<type_symbol> ty)
		: const_typed_symbol(id, ty) {
	}

	builtin_typed_symbol::~builtin_typed_symbol() { }

	// Variable symbol

	var_typed_symbol::var_typed_symbol(ystr const& id, yshared_ptr<type_symbol> ty)
		: typed_symbol(id, ty) {
	}

	var_typed_symbol::~var_typed_symbol() { }
}
