#pragma once

#include "../common.h"

struct ir_basic_block;
struct ir_type;
struct ir_alloca_instr;

struct ir_function_decl {
	ystr					Name;
	yvec<ir_alloca_instr*>	Params;
	ir_type*				ReturnType;

	ir_function_decl(ystr const& name, yvec<ir_alloca_instr*> const& params, ir_type* rett);
	~ir_function_decl();
};
