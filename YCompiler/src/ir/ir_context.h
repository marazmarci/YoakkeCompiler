#pragma once

#include "../common.h"

struct ir_function;

struct ir_context {
	yvec<ir_function*> Functions;

	void add_function(ir_function* fn);
};
