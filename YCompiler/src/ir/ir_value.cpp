#include "ir_type.h"
#include "ir_value.h"

ir_value::ir_value(ir_sized_type* t)
	: Type(t) {
}

ir_value::~ir_value() { }

/*****************************************************************************/

ir_named_value::ir_named_value(ir_sized_type* ty, ystr const& name)
	: ir_value(ty), Name(name) {
}

ir_named_value::~ir_named_value() { }

/*****************************************************************************/

ir_integer_value::ir_integer_value(ir_integer_type* ty, long long int val)
	: ir_value(ty), Value(val) {
}

ir_integer_value* ir_integer_value::create(ysize bits, bool sign, long long int val) {
	return new ir_integer_value(
		new ir_integer_type(bits, sign),
		val
	);
}

ir_integer_value* ir_integer_value::create_32(bool sign, long long int val) {
	return new ir_integer_value(
		new ir_integer_type(32, sign),
		val
	);
}

ir_integer_value* ir_integer_value::create_i32(long long int val) {
	return new ir_integer_value(
		new ir_integer_type(32, true),
		val
	);
}
