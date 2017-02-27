#include "ir_instr.h"
#include "ir_type.h"
#include "ir_function.h"

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

	// Call instruction
	ir_call_instr::ir_call_instr(ir_function_proto* f, yvec<ir_value*> const& a)
		: ir_instr(ir_opcode::call), ir_value(f->ReturnType), FP(f), Args(a) {
	}

	ir_call_instr::~ir_call_instr() { }
}