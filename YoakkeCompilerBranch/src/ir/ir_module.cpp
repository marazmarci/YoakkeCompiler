#include "ir_module.h"
#include "ir_function.h"

namespace yk {
	ir_module::ir_module() { }

	ir_module::~ir_module() {
		for (auto f : Functions) delete f;
	}

	void ir_module::add(ir_function* func) {
		auto proto = func->Prototype;
		proto->Module = this;
		proto->Name = NameGen.get(proto->Name);
		Functions.push_back(func);
	}
}