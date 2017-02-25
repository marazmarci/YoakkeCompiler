#include "symbol.h"

namespace yk {
	symbol::symbol(ystr const& ident)
		: Identifier(ident) {
	}

	symbol::~symbol() { }
}