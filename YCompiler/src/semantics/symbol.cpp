#include "symbol.h"
#include "type.h"

symbol::symbol(symbol_t t, type* ty, ystr const& name)
	: Ty(t), Type(ty), Name(name), DefPos({}) {
}

symbol::~symbol() { }

/*****************************************************************************/

const_symbol::const_symbol(ystr const& name, type* typ)
	: symbol(symbol_t::Constant, typ, name) {
}

const_symbol::~const_symbol() { }

/*****************************************************************************/

var_symbol::var_symbol(ystr const& name, type* typ)
	: symbol(symbol_t::Variable, typ, name) {
}

var_symbol::~var_symbol() { }

/*****************************************************************************/

typeclass_symbol::typeclass_symbol(ystr const& name)
	: symbol(symbol_t::Typeclass, nullptr, name) {
	Type = new type_set(Types);
}

typeclass_symbol::~typeclass_symbol() { }

void typeclass_symbol::add(type* ty) {
	// TODO: Check for ambiguity
	Types.push_back(ty);
}
