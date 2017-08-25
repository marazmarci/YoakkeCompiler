#include "symbol.h"
#include "type.h"

symbol::symbol(symbol_t t, ystr const& name)
	: Ty(t), Name(name), DefPos({}) {
}

symbol::~symbol() { }

/*****************************************************************************/

const_symbol::const_symbol(ystr const& name, type* typ)
	: symbol(symbol_t::Constant, name), Type(typ) {
}

const_symbol::~const_symbol() { }

/*****************************************************************************/

var_symbol::var_symbol(ystr const& name, type* typ)
	: symbol(symbol_t::Variable, name), Type(typ) {
}

var_symbol::~var_symbol() { }

/*****************************************************************************/

typeclass_symbol::typeclass_symbol(ystr const& name, type_cons* typ1, type_cons* typ2)
	: symbol(symbol_t::Typeclass, name), Instances({ typ1, typ2 }),
	Type(new type_set(Instances)) {
}

typeclass_symbol::~typeclass_symbol() { }

void typeclass_symbol::add(type_cons* t) {
	Instances.push_back(t);
}
