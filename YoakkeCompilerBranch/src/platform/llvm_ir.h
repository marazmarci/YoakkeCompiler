#pragma once

#include <iostream>

namespace yk {
	class ir_module;
	class ir_function;
	class ir_function_proto;
	class ir_basic_block;
	class ir_instr;
	class ir_value;

	class llvm_ir {
	private:
		std::ostream& m_Ostream;

	public:
		llvm_ir(std::ostream& os);

	public:
		void print(ir_module* mod);
		void print(ir_function* fn);
		void print(ir_function_proto* pr);
		void print(ir_basic_block* bb);
		void print(ir_instr* ins);

	private:
		void print(ir_value* val);
	};
}
