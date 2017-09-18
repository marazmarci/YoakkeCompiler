#pragma once

#include "../common.h"

struct ir_basic_block;

struct ir_function {
	ystr					Name;
	yvec<ir_basic_block*>	Blocks;
	ysize					Iter;

	ir_function(ystr const& name);
	~ir_function();

	void add_basic_block(ir_basic_block* bb);
	ysize get_point();
	void set_point(ysize p);
};
