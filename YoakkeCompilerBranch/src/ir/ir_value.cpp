#include "ir_value.h"
#include "ir_environment.h"
#include "ir_type.h"

namespace yk {
	ir_value::ir_value(ir_type* t)
		: Type(t) {
	}

	ir_value::~ir_value() { }

	ir_int_value::ir_int_value(int N, int val) 
		: ir_literal_value<int>(nullptr, val) {
		auto t = ir_environment::get_type("int" + std::to_string(N));
		if (t == nullptr) {
			t = new ir_int_type(N);
			ir_environment::add_type(t);
		}
		Type = t;
	}

	ir_int_value::~ir_int_value() { }
}