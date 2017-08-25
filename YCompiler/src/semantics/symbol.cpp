#include "symbol.h"
#include "type.h"

symbol::symbol(symbol_t t, ystr const& name)
	: Ty(t), Name(name) {
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

typeclass_symbol::typeclass_symbol(ystr const& name, type* typ1, type* typ2)
	: symbol(symbol_t::Typeclass, name), Types({ typ1, typ2 }) {
}

typeclass_symbol::~typeclass_symbol() { }
