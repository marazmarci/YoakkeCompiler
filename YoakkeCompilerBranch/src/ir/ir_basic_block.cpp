#include "ir_basic_block.h"
#include "ir_instr.h"
#include "ir_function.h"

namespace yk {
	ir_basic_block::ir_basic_block(ystr const& name) 
		: Name(name), Function(nullptr) {
	}

	ir_basic_block::~ir_basic_block() {
		for (auto ins : Instructions) delete ins;
	}

	void ir_basic_block::add(ir_instr* ins) {
		if (auto alloc = dynamic_cast<ir_alloc_instr*>(ins)) {
			alloc->Name = Function->NameGen.get(alloc->Name);
		}

		Instructions.push_back(ins);
	}
}