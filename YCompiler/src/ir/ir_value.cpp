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
		ir_integer_type::get(bits, sign),
		val
	);
}

ir_integer_value* ir_integer_value::create_32(bool sign, long long int val) {
	return new ir_integer_value(
		ir_integer_type::get(32, sign),
		val
	);
}

ir_integer_value* ir_integer_value::create_i32(long long int val) {
	return new ir_integer_value(
		ir_integer_type::I32,
		val
	);
}

ir_integer_value* ir_integer_value::create_i8(long long int val) {
	return new ir_integer_value(
		ir_integer_type::I8,
		val
	);
}

/*****************************************************************************/

ir_fp_value::ir_fp_value(ir_fp_type* ty, long double val)
	: ir_value(ty), Value(val) {
}

ir_fp_value* ir_fp_value::create(ysize bits, long double val) {
	return new ir_fp_value(
		ir_fp_type::get(bits),
		val
	);
}

ir_fp_value* ir_fp_value::create_32(long long int val) {
	return new ir_fp_value(
		ir_fp_type::F32,
		val
	);
}
