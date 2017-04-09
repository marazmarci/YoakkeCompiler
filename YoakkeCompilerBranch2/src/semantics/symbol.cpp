#include "symbol.h"

namespace yk {
	symbol::symbol(ystr const& id)
		: Identifier(id) {
	}

	symbol::~symbol() { }
}
