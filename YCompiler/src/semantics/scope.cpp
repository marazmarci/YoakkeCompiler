#include "scope.h"

scope::scope()
	: Parent(nullptr) {
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
	auto it = Types.find(name);
	if (it == Types.end()) {
		return {};
	}
	return it->second;
}

yopt<symbol*> scope::local_ref_sym(ystr const& name) {
	auto it = Symbols.find(name);
	if (it == Symbols.end()) {
		return {};
	}
	return it->second;
}

void scope::decl_type(type* ty) {

}

void scope::decl_sym(symbol* sym) {

}
