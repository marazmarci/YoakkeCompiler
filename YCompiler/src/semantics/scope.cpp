#include "scope.h"

scope::scope(bool retDest)
	: Parent(nullptr), ReturnDest(retDest), ReturnType(nullptr), ReturnPos(interval()) {
	if (retDest) {
		ReturnType = new type_var();
	}
}

scope::~scope() { }

yopt<type*> scope::ref_type(ystr const& name) {
	if (auto r = local_ref_type(name)) {
		return r;
	}
	if (Parent) {
		return Parent->ref_type(name);
	}
	return {};
}

yopt<symbol*> scope::ref_sym(ystr const& name) {
	if (auto r = local_ref_sym(name)) {
		return r;
	}
	if (Parent) {
		return Parent->ref_sym(name);
	}
	return {};
}

yopt<type*> scope::local_ref_type(ystr const& name) {
	return local_ref(Types, name);
}

yopt<symbol*> scope::local_ref_sym(ystr const& name) {
	return local_ref(Symbols, name);
}

void scope::decl_type(ystr const& name, type* ty) {
	decl(Types, name, ty);
}

void scope::decl_sym(symbol* sym) {
	decl(Symbols, sym->Name, sym);
}

yopt<symbol*> scope::remove_symbol(ystr const& name) {
	return remove(Symbols, name);
}

yopt<type*> scope::remove_type(ystr const& name) {
	return remove(Types, name);
}

bool scope::shadow_symbol(ystr const& name) {
	return shadow(Symbols, name);
}

bool scope::shadow_type(ystr const& name) {
	return shadow(Types, name);
}
