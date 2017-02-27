#include "ir_function.h"
#include "ir_basic_block.h"
#include "ir_type.h"

namespace yk {
	ir_parameter::ir_parameter(ystr const& name, ir_type* t)
		: Name(name), Type(t) {
	}

	ir_parameter::~ir_parameter() { }

	ir_function_proto::ir_function_proto(ystr const& id, ir_type* rett, yvec<ir_parameter*> const& params)
		: ir_value(nullptr), Name(id), Module(nullptr), ReturnType(rett), Parameters(params) {
		for (auto p : Parameters) {
			p->Name = NameGen.get(p->Name);
		}
		yvec<ir_type*> pars;
		for (auto par : Parameters) {
			pars.push_back(par->Type);
		}
		Type = new ir_func_type(pars, ReturnType);
	}

	ir_function_proto::~ir_function_proto() { }

	ir_function::ir_function(ir_function_proto* proto)
		: ir_value(proto->Type), Prototype(proto), NameGen(proto->NameGen) {
	}

	ir_function::~ir_function() {
		for (auto bb : Blocks) delete bb;
	}

	void ir_function::add(ir_basic_block* blck) {
		blck->Name = NameGen.get(blck->Name);
		blck->Function = this;
		Blocks.push_back(blck);
	}
}