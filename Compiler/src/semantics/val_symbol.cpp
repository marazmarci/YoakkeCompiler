#include "val_symbol.h"

// val_symbol

val_symbol::val_symbol(ystr const& name, val_symbol_t ty, ty_symbol* symt)
	: symbol(name), Category(ty), Type(symt) {
}

val_symbol::~val_symbol() { }

// var_val_symbol

var_val_symbol::var_val_symbol(ystr const& name, ty_symbol* symt)
	: val_symbol(name, val_symbol_t::Var, symt) {
}

var_val_symbol::~var_val_symbol() { }

// const_val_symbol

const_val_symbol::const_val_symbol(ystr const& name, ty_symbol* symt)
	: val_symbol(name, val_symbol_t::Const, symt) {
}

const_val_symbol::~const_val_symbol() { }
