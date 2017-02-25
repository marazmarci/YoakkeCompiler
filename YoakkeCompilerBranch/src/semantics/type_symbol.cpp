#include "type_symbol.h"

namespace yk {
	type_symbol::type_symbol(ystr const& id)
		: symbol(id) {
	}

	type_symbol::~type_symbol() { }

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
}