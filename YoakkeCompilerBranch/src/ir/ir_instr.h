#pragma once

#include "ir_opcode.h"

namespace yk {
	class ir_basic_block;
	class ir_value;

	class ir_instr {
	public:
		ir_opcode Opcode;

	protected:
		ir_instr(ir_opcode code);

	public:
		virtual ~ir_instr();
	};

	class ir_ret_instr : public ir_instr {
	public:
		ir_value* Value;

	public:
		ir_ret_instr(ir_value* val = nullptr);
		~ir_ret_instr();
	};

	class ir_jmp_instr : public ir_instr {
	public:
		ir_basic_block* Destination;

	public:
		ir_jmp_instr(ir_basic_block* dest);
		~ir_jmp_instr();
	};
}
