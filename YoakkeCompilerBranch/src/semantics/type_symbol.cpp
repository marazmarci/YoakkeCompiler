#include "type_symbol.h"

namespace yk {
	type_symbol::type_symbol(ystr const& id)
		: symbol(id) {
	}

	type_symbol::~type_symbol() { }

	// Builtin type symbol
	builtin_type_symbol::builtin_type_symbol(ystr const& id)
		: type_symbol(id) {
	}
	
	builtin_type_symbol::~builtin_type_symbol() { }

	bool builtin_type_symbol::match(type_symbol* other) {
		if (auto o2 = dynamic_cast<builtin_type_symbol*>(other)) {
			return o2->Identifier == Identifier;
		}
		return false;
	}

	// Tuple type symbol
	tuple_type_symbol::tuple_type_symbol(yvec<type_symbol*> const& ts)
		: type_symbol("tuple"), Types(ts) {
	}

	tuple_type_symbol::~tuple_type_symbol() { }

	bool tuple_type_symbol::match(type_symbol* other) {
		if (auto o2 = dynamic_cast<tuple_type_symbol*>(other)) {
			if (o2->Types.size() != Types.size()) {
				return false;
			}
			for (ysize i = 0; i < Types.size(); i++) {
				if (!(Types[i]->match(o2->Types[i]))) {
					return false;
				}
			}
			return true;
		}
		return false;
	}
}