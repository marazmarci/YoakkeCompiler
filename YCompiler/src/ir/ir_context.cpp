#include "ir_context.h"

void ir_context::add_function_decl(ir_function_decl* decl) {
	FunctionDecls.push_back(decl);
}

void ir_context::add_function(ir_function* fn) {
	Functions.push_back(fn);
}
