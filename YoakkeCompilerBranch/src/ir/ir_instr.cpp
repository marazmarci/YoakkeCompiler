#include "ir_instr.h"
#include "ir_type.h"

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
		: ir_instr(ir_opcode::alloc), ir_named_value(new ir_ptr_type(t), name), What(t) {
	}

	ir_alloc_instr::~ir_alloc_instr() { }

	// Store instruction
	ir_store_instr::ir_store_instr(ir_value* p, ir_value* v)
		: ir_instr(ir_opcode::store), Ptr(p), Value(v) {
	}

	ir_store_instr::~ir_store_instr() { }
}