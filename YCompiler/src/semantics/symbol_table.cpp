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
