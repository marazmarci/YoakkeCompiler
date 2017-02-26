#include "ir_instr.h"

namespace yk {
	ir_instr::ir_instr(ir_opcode code)
		: Opcode(code) {
	}

	ir_instr::~ir_instr() { }

	// Return instruction
	ir_ret_instr::ir_ret_instr(ir_value* val)
		: ir_instr(ir_opcode::ret), Value(val) {
	}

	ir_ret_instr::~ir_ret_instr() { }

	// Jump instruction
	ir_jmp_instr::ir_jmp_instr(ir_basic_block* dest)
		: ir_instr(ir_opcode::jmp), Destination(dest) {
	}

	ir_jmp_instr::~ir_jmp_instr() { }

	// Alloca instr
	ir_alloc_instr::ir_alloc_instr(ystr const& name, ir_type* t)
		: ir_instr(ir_opcode::alloc), ir_value(t) {
	}

	ir_alloc_instr::~ir_alloc_instr() { }
}