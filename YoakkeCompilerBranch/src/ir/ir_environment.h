#pragma once

#include "../common.h"

namespace yk {
	class ir_module;
	class ir_type;

	class ir_environment {
	public:
		static ir_module* CurrentModule;
		static yvec<ir_module*> Modules;
		static ymap<ystr, ir_type*> TypeTable;

	public:
		static void init();
		static void deinit();

	public:
		static void add_module(ir_module* mods);
		static void add_type(ir_type* t);
		static ir_type* get_type(ystr const& id);

	private:
		ir_environment() = delete;
	};
}
