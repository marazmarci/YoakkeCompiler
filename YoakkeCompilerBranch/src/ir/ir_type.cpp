#include "ir_type.h"

namespace yk {
	ir_type::ir_type(ystr const& ident, ysize sz) 
		: Identifier(ident), Size(sz) {
	}

	ir_type::~ir_type() { }

	// Void
	ir_void_type::ir_void_type()
		: ir_type("void", 0) {
	}

	ir_void_type::~ir_void_type() { }

	// Int (n bits)
	ir_int_type::ir_int_type(ysize sz)
		: ir_type("i" + std::to_string(sz), sz) {
	}

	ir_int_type::~ir_int_type() { }

	// TODO
	// Function
	ir_func_type::ir_func_type(yvec<ir_type*> const& params, ir_type* rett)
		: ir_type("func", 0), Parameters(params), ReturnType(rett) {
	}

	ir_func_type::~ir_func_type() { }
}