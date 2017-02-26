#pragma once

#include "../common.h"

namespace yk {
	class ir_type {
	public:
		ystr Identifier;
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
}
