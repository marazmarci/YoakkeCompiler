#pragma once

#include "../common.h"
#include "../utility/unique_name_gen.h"
#include "ir_value.h"

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

	class ir_function_proto : public ir_value {
	public:
		unique_name_gen NameGen;
		ystr Name;
		ir_module* Module;
		ir_type* ReturnType;
		yvec<ir_parameter*> Parameters;

	public:
		ir_function_proto(ystr const& id, ir_type* rett, yvec<ir_parameter*> const& params);
		virtual ~ir_function_proto();
	};

	class ir_function : public ir_value {
	public:
		ir_function_proto* Prototype;
		yvec<ir_basic_block*> Blocks;
		unique_name_gen& NameGen;

	public:
		ir_function(ir_function_proto* proto);
		~ir_function();

	public:
		void add(ir_basic_block* blck);
	};
}
