#pragma once

#include "../common.h"

namespace yk {
	class ir_type;

	class ir_value {
	public:
		ir_type* Type;

	protected:
		ir_value(ir_type* t);

	public:
		virtual ~ir_value();
	};

	template <typename T>
	class ir_literal_value : public ir_value {
	public:
		T Value;

	protected:
		ir_literal_value(ir_type* t, T const& v)
			: ir_value(t), Value(v) {
		}

	public:
		virtual ~ir_literal_value() { }
	};

	class ir_int_value : public ir_literal_value<int> {
	public:
		ir_int_value(int N, int val);
		virtual ~ir_int_value();
	};

	class ir_real_value : public ir_literal_value<double> {
	public:
		ir_real_value(int N, double val);
		virtual ~ir_real_value();
	};

	class ir_named_value : public ir_value {
	public:
		ystr Name;

	protected:
		ir_named_value(ir_type* t, ystr const& n);

	public:
		virtual ~ir_named_value();
	};

	class ir_parameter : public ir_named_value {
	public:
		ir_parameter(ystr const& name, ir_type* t);
		virtual ~ir_parameter();
	};
}
