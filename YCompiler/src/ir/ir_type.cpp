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

ir_integer_type* ir_integer_type::I32 = new ir_integer_type(32, true);
ir_integer_type* ir_integer_type::U32 = new ir_integer_type(32, false);
ir_integer_type* ir_integer_type::I8 = new ir_integer_type(8, true);
ir_integer_type* ir_integer_type::U8 = new ir_integer_type(8, false);

ir_integer_type::ir_integer_type(ysize bits, bool sig) 
	: ir_sized_type(ir_type_t::Integer, bits), Signed(sig) {
}

ir_integer_type::~ir_integer_type() { }

ir_integer_type* ir_integer_type::get(ysize bits, bool sig) {
	switch (bits) {
	case 8:		return sig ? I8		: U8;
	case 32:	return sig ? I32	: U32;

	default:	UNIMPLEMENTED;
	}
	UNREACHABLE;
}

/*****************************************************************************/

ir_fp_type* ir_fp_type::F32 = new ir_fp_type(32);

ir_fp_type::ir_fp_type(ysize bits)
	: ir_sized_type(ir_type_t::FloatingPoint, bits) {
}

ir_fp_type::~ir_fp_type() { }

ir_fp_type* ir_fp_type::get(ysize bits) {
	switch (bits) {
	case 32:	return F32;

	default: UNIMPLEMENTED;
	}
	UNREACHABLE;
}
