#include "ir_basic_block.h"
#include "ir_instr.h"
#include "ir_function.h"

namespace yk {
	ir_basic_block::ir_basic_block(ystr const& name) 
		: Name(name), Function(nullptr) {
	}

	ir_basic_block::~ir_basic_block() {
		// TODO
		//for (auto ins : Instructions) delete ins;
	}

	void ir_basic_block::add(ir_instr* ins) {
		rename_instr(ins);
		Instructions.push_back(ins);
	}

	void ir_basic_block::add_begin(ir_instr* ins) {
		rename_instr(ins);
		if (Instructions.size()) {
			Instructions.insert(Instructions.begin(), ins);
		}
		else {
			Instructions.push_back(ins);
		}
	}

	void ir_basic_block::rename_instr(ir_instr* ins) {
		if (auto named = dynamic_cast<ir_named_value*>(ins)) {
			named->Name = Function->NameGen.get(named->Name);
		}
	}
}