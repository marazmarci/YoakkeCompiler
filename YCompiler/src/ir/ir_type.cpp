#include "ir_type.h"

ir_type::ir_type(ir_type_t t)
	: Ty(t) {
}

ir_type::~ir_type() { }

/*****************************************************************************/

ir_void_type::ir_void_type()
	: ir_type(ir_type_t::Void) {
}

ir_void_type::~ir_void_type() { }

/*****************************************************************************/

ir_sized_type::ir_sized_type(ir_type_t t, ysize bits)
	: ir_type(t), Bits(bits) {
}

ir_sized_type::~ir_sized_type() { }

/*****************************************************************************/

ir_integer_type::ir_integer_type(ysize bits, bool sig) 
	: ir_sized_type(ir_type_t::Integer, bits), Signed(sig) {
}

ir_integer_type::~ir_integer_type() { }

/*****************************************************************************/

ir_fp_type::ir_fp_type(ysize bits)
	: ir_sized_type(ir_type_t::FloatingPoint, bits) {
}

ir_fp_type::~ir_fp_type() { }
