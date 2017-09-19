#pragma once

#include "ir_opcode.h"
#include "ir_type.h"
#include "ir_value.h"

struct ir_instr {
public:
	ir_opcode Opcode;

protected:
	ir_instr(ir_opcode opcode);

public:
	virtual ~ir_instr();
};

struct ir_alloca_instr : public ir_instr, public ir_named_value {
	ir_alloca_instr(ir_sized_type* ty, ystr name);
	virtual ~ir_alloca_instr();
};
