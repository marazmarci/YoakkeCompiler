#pragma once

#include "../common.h"
#include "../utility/unique_name_gen.h"

namespace yk {
	class ir_module;
	class ir_basic_block;

	class ir_function {
	public:
		ystr Name;
		yvec<ir_basic_block*> Blocks;
		unique_name_gen NameGen;
		ir_module* Module;

	public:
		ir_function(ystr const& name);
		~ir_function();

	public:
		void add(ir_basic_block* blck);
	};
}
