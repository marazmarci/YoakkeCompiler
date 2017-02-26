#pragma once

#include "../utility/unique_name_gen.h"

namespace yk {
	class ir_function;

	class ir_module {
	public:
		unique_name_gen NameGen;
		yvec<ir_function*> Functions;

	public:
		ir_module();
		virtual ~ir_module();

	public:
		void add(ir_function* func);
	};
}
