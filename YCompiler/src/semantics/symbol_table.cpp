#include "scope.h"
#include "symbol_table.h"

symbol_table::symbol_table()
	: Global(nullptr), Current(nullptr) {
	Global = new scope();
	Current = Global;
}

symbol_table::~symbol_table() { }

void symbol_table::push_scope() {
	scope* sc = new scope();
	sc->Parent = Current;
	Current = sc;
}

void symbol_table::pop_scope() {
	assert(Current->Parent && "Tried to pop a scope with no parent!");

	Current = Current->Parent;
}

void symbol_table::decl(type_cons* tc) {
	Current->decl_type(tc->Name, tc);
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
