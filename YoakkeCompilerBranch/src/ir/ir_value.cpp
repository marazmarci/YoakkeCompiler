#include "ir_value.h"
#include "ir_environment.h"
#include "ir_type.h"

namespace yk {
	ir_value::ir_value(ir_type* t)
		: Type(t) {
	}

	ir_value::~ir_value() { }

	// TODO: automatize this in the ir type table
	ir_int_value::ir_int_value(int N, int val) 
		: ir_literal_value<int>(nullptr, val) {
		auto t = ir_environment::get_type("i" + std::to_string(N));
		if (t == nullptr) {
			t = new ir_int_type(N);
			ir_environment::add_type(t);
		}
		Type = t;
	}

	ir_int_value::~ir_int_value() { }

	// TODO: same as above
	ir_real_value::ir_real_value(int N, double val)
		: ir_literal_value<double>(nullptr, val) {
		auto t = ir_environment::get_type("f" + std::to_string(N));
		if (t == nullptr) {
			t = new ir_int_type(N);
			ir_environment::add_type(t);
		}
		Type = t;
	}

	ir_real_value::~ir_real_value() { }

	ir_named_value::ir_named_value(ir_type* t, ystr const& n)
		: ir_value(t), Name(n) {
	}

	ir_named_value::~ir_named_value() { }

	ir_parameter::ir_parameter(ystr const& name, ir_type* t)
		: ir_named_value(t, name) {
	}

	ir_parameter::~ir_parameter() { }
}