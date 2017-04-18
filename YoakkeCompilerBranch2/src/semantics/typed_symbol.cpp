#include "typed_symbol.h"

namespace yk {
	// Abstract typed symbol

	typed_symbol::typed_symbol(ystr const& id, ysptr<type_symbol> ty)
		: symbol(id), Type(ty) {
	}

	typed_symbol::~typed_symbol() { }

	// Constant symbol

	ystr const_typed_symbol::create_preury_op_name(ytoken_t op) {
		return "@pre_op" + std::to_string((ysize)op);
	}

	ystr const_typed_symbol::create_postury_op_name(ytoken_t op) {
		return "@post_op" + std::to_string((ysize)op);
	}

	ystr const_typed_symbol::create_bin_op_name(ytoken_t op) {
		return "@bin_op" + std::to_string((ysize)op);
	}

	const_typed_symbol::const_typed_symbol(ystr const& id, ysptr<type_symbol> ty)
		: typed_symbol(id, ty) {
	}

	const_typed_symbol::~const_typed_symbol() { }

	// Builtin symbol (like an operator)

	ysptr<builtin_typed_symbol> builtin_typed_symbol::create_preury_op(ytoken_t op, ysptr<type_symbol> ty) {
		return std::make_shared<builtin_typed_symbol>
			(const_typed_symbol::create_preury_op_name(op), ty);
	}

	ysptr<builtin_typed_symbol> builtin_typed_symbol::create_postury_op(ytoken_t op, ysptr<type_symbol> ty) {
		return std::make_shared<builtin_typed_symbol>
			(const_typed_symbol::create_postury_op_name(op), ty);
	}

	ysptr<builtin_typed_symbol> builtin_typed_symbol::create_bin_op(ytoken_t op, ysptr<type_symbol> ty) {
		return std::make_shared<builtin_typed_symbol>
			(const_typed_symbol::create_bin_op_name(op), ty);
	}

	builtin_typed_symbol::builtin_typed_symbol(ystr const& id, ysptr<type_symbol> ty)
		: const_typed_symbol(id, ty) {
	}

	builtin_typed_symbol::~builtin_typed_symbol() { }

	// Variable symbol

	var_typed_symbol::var_typed_symbol(ystr const& id, ysptr<type_symbol> ty)
		: typed_symbol(id, ty) {
	}

	var_typed_symbol::~var_typed_symbol() { }
}
