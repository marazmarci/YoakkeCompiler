#pragma once

#include "ir_opcode.h"
#include "ir_value.h"

namespace yk {
	class ir_basic_block;
	class ir_value;
	class ir_function_proto;

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

	class ir_alloc_instr : public ir_instr, public ir_named_value {
	public:
		ir_type* What;

	public:
		ir_alloc_instr(ystr const& name, ir_type* t);
		~ir_alloc_instr();
	};

	class ir_store_instr : public ir_instr {
	public:
		ir_value* Ptr;
		ir_value* Value;

	public:
		ir_store_instr(ir_value* p, ir_value* v);
		~ir_store_instr();
	};

	class ir_call_instr : public ir_instr, public ir_value {
	public:
		ir_function_proto* FP;
		yvec<ir_value*> Args;

	public:
		ir_call_instr(ir_function_proto* f, yvec<ir_value*> const& a);
		~ir_call_instr();
	};
}
