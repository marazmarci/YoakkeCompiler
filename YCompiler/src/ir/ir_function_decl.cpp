#include "ir_function_decl.h"

ir_function_decl::ir_function_decl(ystr const& name, yvec<ir_alloca_instr*> const& params, ir_type* rett)
	: Name(name), Params(params), ReturnType(rett) {
}

ir_function_decl::~ir_function_decl() { }
