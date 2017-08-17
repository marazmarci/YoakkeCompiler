#include "scope.h"

scope::scope()
	: Parent(nullptr) {
}

scope::~scope() { }

yopt<type*> scope::ref_type(ystr const& name) {
	return ref(Types, name);
}

yopt<symbol*> scope::ref_sym(ystr const& name) {
	return ref(Symbols, name);
}

yopt<type*> scope::local_ref_type(ystr const& name) {
	return local_ref(Types, name);
}

yopt<symbol*> scope::local_ref_sym(ystr const& name) {
	return local_ref(Symbols, name);
}

void scope::decl_type(ystr const& name, type* ty) {
	assert(ty->Ty != type_t::Variable && "Cannot add a type variable");
	decl(Types, name, ty);
}

void scope::decl_sym(symbol* sym) {
	decl(Symbols, sym->Name, sym);
}

void scope::shadow_symbol(ystr const& name) {
	shadow(Symbols, name);
}

void scope::shadow_type(ystr const& name) {
	shadow(Types, name);
}
