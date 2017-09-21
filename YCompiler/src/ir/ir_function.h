#pragma once

#include "../common.h"

struct ir_basic_block;
struct ir_type;
struct ir_alloca_instr;
struct ir_function_decl;

struct ir_function {
	ir_function_decl*		Decl;
	yvec<ir_basic_block*>	Blocks;
	ysize					Iter;

	ir_function(ir_function_decl* decl);
	~ir_function();

	void add_basic_block(ir_basic_block* bb);
	ysize get_point();
	void set_point(ysize p);
};
