#include "ir_context.h"

void ir_context::add_function(ir_function* fn) {
	Functions.push_back(fn);
}
