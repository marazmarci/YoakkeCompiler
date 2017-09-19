#include "ir_instr.h"

ir_instr::ir_instr(ir_opcode opcode)
	: Opcode(opcode) {
}

ir_instr::~ir_instr() { }

/*****************************************************************************/

ir_alloca_instr::ir_alloca_instr(ir_sized_type* ty, ystr name) 
	: ir_instr(ir_opcode::Alloca), ir_named_value(ty, name) {
}

ir_alloca_instr::~ir_alloca_instr() { }
