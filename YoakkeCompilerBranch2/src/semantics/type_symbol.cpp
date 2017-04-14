#include "type_symbol.h"

namespace yk {
	type_symbol::type_symbol(ystr const& id)
		: symbol(id) {
	}

	type_symbol::~type_symbol() { }

	// Native type symbol

	native_type_symbol::native_type_symbol(ystr const& id)
		: type_symbol(id) {
	}

	native_type_symbol::~native_type_symbol() { }

	// Function type symbol

	ystr fn_type_symbol::create_name(yshared_ptr<type_symbol> p, yshared_ptr<type_symbol> r) {
		return "((" + p->Identifier + ")->(" + r->Identifier + "))";
	}

	fn_type_symbol::fn_type_symbol(yshared_ptr<type_symbol> p, yshared_ptr<type_symbol> r)
		: type_symbol(create_name(p, r)), Parameters(p), Return(r) {
	}
	
	fn_type_symbol::~fn_type_symbol() { }

	// Tuple type symbol

	ystr tuple_type_symbol::create_name(yvec<yshared_ptr<type_symbol>> const& ls) {
		ystr res = "(";
		if (ls.size()) {
			res += ls[0]->Identifier;
			for (std::size_t i = 1; i < ls.size(); i++) {
				res += "," + ls[i]->Identifier;
			}
		}
		res += ')';
		return res;
	}

	tuple_type_symbol::tuple_type_symbol(yvec<yshared_ptr<type_symbol>>& ls)
		: type_symbol(create_name(ls)), Elements(ls) {
	}

	tuple_type_symbol::~tuple_type_symbol() { }
}
