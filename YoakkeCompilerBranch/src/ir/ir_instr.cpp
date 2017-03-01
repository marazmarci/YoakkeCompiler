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
	ir_call_instr::ir_call_instr(ystr const& name, ir_function_proto* f, yvec<ir_value*> const& a)
		: ir_instr(ir_opcode::call), ir_named_value(f->ReturnType, name), FP(f), Args(a) {
	}

	ir_call_instr::~ir_call_instr() { }

	// Load instruction
	ir_load_instr::ir_load_instr(ystr const& name, ir_value* p) 
		: ir_instr(ir_opcode::load), ir_named_value(nullptr, name), Ptr(p) {
		if (auto ptrt = dynamic_cast<ir_ptr_type*>(p->Type)) {
			Type = ptrt->Sub;
		}
		else {
			throw std::exception("Can not load non-ptr type!");
		}
	}

	ir_load_instr::~ir_load_instr() { }
}