#pragma once

#include "../common.h"

namespace yk {
	class ir_instr;
	class ir_function;

	class ir_basic_block {
	public:
		ystr Name;
		yvec<ir_instr*> Instructions;
		ir_function* Function;

	public:
		ir_basic_block(ystr const& name);
		~ir_basic_block();

	public:
		void add(ir_instr* ins);
	};
}
