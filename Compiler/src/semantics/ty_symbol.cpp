#include "ty_symbol.h"

// ty_symbol

ty_symbol::ty_symbol(ystr const& name, ty_symbol_t ty)
	: symbol(name), Type(ty) {
}

ty_symbol::~ty_symbol() { }

// construct_ty_symbol

construct_ty_symbol::construct_ty_symbol(ystr const& name, yvec<ty_symbol*> const& tys)
	: ty_symbol(name, ty_symbol_t::Construct), Subtypes(tys) {
	if (Subtypes.size()) {
		Name += '<' + Subtypes[0]->Name;
		for (ysize i = 1; i < Subtypes.size(); i++) {
			Name += Subtypes[i]->Name;
		}
		Name += '>';
	}
}

construct_ty_symbol::~construct_ty_symbol() { }
