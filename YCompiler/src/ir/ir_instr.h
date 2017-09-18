#pragma once

#include "ir_opcode.h"

struct ir_instr {
public:
	ir_opcode Opcode;

protected:
	ir_instr(ir_opcode opcode);

public:
	virtual ~ir_instr();
};
