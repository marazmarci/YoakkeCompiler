#include "symbol.h"

namespace yk {
	symbol::symbol(ystr const& ident)
		: identifier(ident) {
	}

	symbol::~symbol() { }
}