#include "symbol.h"

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

overload_set_symbol::overload_set_symbol(ystr const& name, type* typ)
	: symbol(symbol_t::OverloadSet, name), Types{ typ } {
}

overload_set_symbol::~overload_set_symbol() { }
