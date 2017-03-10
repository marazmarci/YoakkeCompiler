#include "ir_environment.h"
#include "ir_type.h"

namespace yk {
	ir_module* ir_environment::CurrentModule;
	yvec<ir_module*> ir_environment::Modules;
	ymap<ystr, ir_type*> ir_environment::TypeTable;

	void ir_environment::init() {
		// Add builtin types for lookup
		ir_environment::add_type(new ir_void_type());
		ir_environment::add_type(new ir_int_type(32));
		ir_environment::add_type(new ir_real_type(32));
	}

	void ir_environment::deinit() {
		for (auto m : Modules) delete m;
	}

	void ir_environment::add_module(ir_module* mods) {
		Modules.push_back(mods);
	}

	void ir_environment::add_type(ir_type* t) {
		TypeTable.insert(std::make_pair(t->identifier, t));
	}

	ir_type* ir_environment::get_type(ystr const& id) {
		auto it = TypeTable.find(id);
		if (it == TypeTable.end()) {
			return nullptr;
		}
		return it->second;
	}
}