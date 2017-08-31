#include "symbol.h"
#include "type.h"
#include "unifier.h"

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

typeclass_symbol::typeclass_symbol(ystr const& name, const_symbol* ins1)
	: symbol(symbol_t::Typeclass, name), Instances({ ins1 }) {
}

typeclass_symbol::~typeclass_symbol() { }

yopt<const_symbol*> typeclass_symbol::add(const_symbol* t) {
	for (auto& ins : Instances) {
		if (unifier::matches_fn_fn(ins->Type, t->Type)) {
			return ins;
		}
	}
	Instances.push_back(t);
	return {};
}
