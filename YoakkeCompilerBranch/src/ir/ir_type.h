#pragma once

#include "../common.h"

namespace yk {
	class ir_type {
	public:
		ystr identifier;
		ysize Size;

	protected:
		ir_type(ystr const& ident, ysize sz);

	public:
		virtual ~ir_type();
	};

	class ir_void_type : public ir_type {
	public:
		ir_void_type();
		virtual ~ir_void_type();
	};

	class ir_int_type : public ir_type {
	public:
		ir_int_type(ysize sz);
		virtual ~ir_int_type();
	};

	class ir_real_type : public ir_type {
	public:
		ir_real_type(ysize sz);
		virtual ~ir_real_type();
	};

	class ir_func_type : public ir_type {
	public:
		yvec<ir_type*> Parameters;
		ir_type* ReturnType;

	public:
		ir_func_type(yvec<ir_type*> const& params, ir_type* rett);
		virtual ~ir_func_type();
	};

	class ir_ptr_type : public ir_type {
	public:
		ir_type* Sub;

	public:
		ir_ptr_type(ir_type* s);
		virtual ~ir_ptr_type();
	};
}
