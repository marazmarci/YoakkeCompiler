#include "ir_basic_block.h"

namespace yk {
	ir_basic_block::ir_basic_block(ystr const& name) 
		: Name(name), Function(nullptr) {
	}

	ir_basic_block::~ir_basic_block() {
		for (auto ins : Instructions) delete ins;
	}

	void ir_basic_block::add(ir_instr* ins) {
		Instructions.push_back(ins);
	}
}