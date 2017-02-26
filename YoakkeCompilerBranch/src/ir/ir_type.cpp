#include "ir_type.h"

namespace yk {
	ir_type::ir_type(ystr const& ident, ysize sz) 
		: Identifier(ident), Size(sz) {
	}

	ir_type::~ir_type() { }

	ir_int_type::ir_int_type(ysize sz)
		: ir_type("i" + std::to_string(sz), sz) {
	}

	ir_int_type::~ir_int_type() { }
}