#pragma once

#include "../common.h"

struct ir_function;
struct ir_function_decl;

struct ir_context {
	yvec<ir_function_decl*> FunctionDecls;
	yvec<ir_function*>		Functions;

	void add_function_decl(ir_function_decl* decl);
	void add_function(ir_function* fn);
};
