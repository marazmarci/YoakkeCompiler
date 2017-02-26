#pragma once

#include "../common.h"
#include "../utility/unique_name_gen.h"

namespace yk {
	class ir_module;
	class ir_basic_block;
	class ir_type;

	class ir_parameter {
	public:
		ystr Name;
		ir_type* Type;

	public:
		ir_parameter(ystr const& name, ir_type* t);
		virtual ~ir_parameter();
	};

	class ir_function {
	public:
		ystr Name;
		yvec<ir_basic_block*> Blocks;
		unique_name_gen NameGen;
		ir_module* Module;
		ir_type* ReturnType;
		yvec<ir_parameter*> Parameters;

	public:
		ir_function(ystr const& name, ir_type* rett, yvec<ir_parameter*> const& params);
		~ir_function();

	public:
		void add(ir_basic_block* blck);
	};
}
