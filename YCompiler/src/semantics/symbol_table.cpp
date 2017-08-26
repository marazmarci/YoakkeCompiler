#include "scope.h"
#include "symbol_table.h"
#include "unifier.h"

symbol_table::symbol_table()
	: Current(nullptr), Global(nullptr) {
	Global = new scope(false, true);
	Current = Global;
}

symbol_table::~symbol_table() { }

scope* symbol_table::push_scope(bool ret, bool free) {
	scope* sc = new scope(ret, free);
	sc->Parent = Current;
	Current = sc;
	return Current;
}

void symbol_table::push_scope(scope* sc) {
	assert(sc->Parent == Current && "Scope corruption!");
	Current = sc;
}

void symbol_table::pop_scope() {
	assert(Current->Parent && "Tried to pop a scope with no parent!");

	Current = Current->Parent;
}

void symbol_table::decl(ystr const& name, type* ty) {
	Current->decl_type(name, ty);
}

void symbol_table::decl(symbol* sym) {
	Current->decl_sym(sym);
}

yopt<symbol*> symbol_table::ref_sym(ystr const& name) {
	return Current->ref_sym(name);
}

yopt<type*> symbol_table::ref_type(ystr const& name) {
	return Current->ref_type(name);
}

yopt<symbol*> symbol_table::local_ref_sym(ystr const& name) {
	return Current->local_ref_sym(name);
}

yopt<type*> symbol_table::local_ref_type(ystr const& name) {
	return Current->local_ref_type(name);
}

yopt<symbol*> symbol_table::upper_ref_sym(ystr const& name) {
	scope* par = Current->Parent;
	if (par) {
		return par->ref_sym(name);
	}
	return {};
}

yopt<type*> symbol_table::upper_ref_type(ystr const& name) {
	scope* par = Current->Parent;
	if (par) {
		return par->ref_type(name);
	}
	return {};
}

yopt<scope*> symbol_table::nearest_ret_dest() {
	scope* sc = Current;
	while (sc) {
		if (sc->ReturnDest) {
			return sc;
		}
		sc = sc->Parent;
	}
	return {};
}

yopt<symbol*> symbol_table::remove_symbol(ystr const& name) {
	return Current->remove_symbol(name);
}

bool symbol_table::shadow_symbol(ystr const& name) {
	return Current->shadow_symbol(name);
}
