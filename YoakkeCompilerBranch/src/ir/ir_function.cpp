#include "ir_function.h"
#include "ir_basic_block.h"

namespace yk {
	ir_parameter::ir_parameter(ystr const& name, ir_type* t)
		: Name(name), Type(t) {
	}

	ir_parameter::~ir_parameter() { }

	ir_function::ir_function(ystr const& name, ir_type* rett, yvec<ir_parameter*> const& params)
		: Name(name), Module(nullptr), ReturnType(rett), Parameters(params) {
		for (auto p : Parameters) {
			p->Name = NameGen.get(p->Name);
		}
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