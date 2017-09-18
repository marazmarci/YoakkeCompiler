#pragma once

#include "../common.h"

struct ir_function;
struct ir_instr;

struct ir_basic_block {
	ystr			Name;
	ir_function*	Function;
	yvec<ir_instr*>	Instructions;
	ysize			Iter;

	ir_basic_block(ystr const& name, ir_function* fn);
	~ir_basic_block();

	void add_instr(ir_instr* ins);
	ysize get_point();
	void set_point(ysize p);
};
