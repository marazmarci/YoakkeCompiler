#include "ir_basic_block.h"

ir_basic_block::ir_basic_block(ystr const& name, ir_function* fn)
	: Name(name), Function(fn), Iter(0) {
}

ir_basic_block::~ir_basic_block() {
	for (auto ins : Instructions) {
		delete ins;
	}
}

void ir_basic_block::add_instr(ir_instr* ins) {
	Instructions.insert(Instructions.begin() + Iter++, ins);
}

ysize ir_basic_block::get_point() {
	return Iter;
}

void ir_basic_block::set_point(ysize p) {
	Iter = p;
}
