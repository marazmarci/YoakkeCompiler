#include "ir_function.h"

ir_function::ir_function(ystr const& name)
	: Name(name), Iter(0) {
}

ir_function::~ir_function() {
	for (auto bb : Blocks) {
		delete bb;
	}
}

void ir_function::add_basic_block(ir_basic_block* bb) {
	Blocks.insert(Blocks.begin() + Iter++, bb);
}

ysize ir_function::get_point() {
	return Iter;
}

void ir_function::set_point(ysize p) {
	Iter = p;
}
