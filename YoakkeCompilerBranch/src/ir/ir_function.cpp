#include "ir_function.h"
#include "ir_basic_block.h"

namespace yk {
	ir_function::ir_function(ystr const& name) 
		: Name(name), Module(nullptr) {
	}

	ir_function::~ir_function() {
		for (auto bb : Blocks) delete bb;
	}

	void ir_function::add(ir_basic_block* blck) {
		blck->Name = NameGen.get(blck->Name);
		blck->Function = this;
		Blocks.push_back(blck);
	}
}