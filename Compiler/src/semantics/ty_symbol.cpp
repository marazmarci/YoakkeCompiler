#include "ty_symbol.h"

// ty_symbol

ty_symbol::ty_symbol(ystr const& name, ty_symbol_t ty)
	: symbol(name), Type(ty) {
}

ty_symbol::~ty_symbol() { }
