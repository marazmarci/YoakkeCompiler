#pragma once

#include <iostream>

namespace yk {
	class ir_module;
	class ir_function;
	class ir_basic_block;
	class ir_instr;

	class ir_printer {
	private:
		std::ostream& m_Ostream;

	public:
		ir_printer(std::ostream& os);

	public:
		void print(ir_module* mod);
		void print(ir_function* fn);
		void print(ir_basic_block* bb);
		void print(ir_instr* ins);
	};
}
