#include "symbol.h"
#include "type.h"

symbol::symbol(symbol_t t, type* ty, ystr const& name)
	: Ty(t), Name(name) {
}

symbol::~symbol() { }

/*****************************************************************************/

const_symbol::const_symbol(ystr const& name, type* typ)
	: symbol(symbol_t::Constant, typ, name), Type(typ) {
}

const_symbol::~const_symbol() { }

/*****************************************************************************/

var_symbol::var_symbol(ystr const& name, type* typ)
	: symbol(symbol_t::Variable, typ, name), Type(typ) {
}

var_symbol::~var_symbol() { }

/*****************************************************************************/
