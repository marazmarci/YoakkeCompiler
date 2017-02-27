#include "type_desc.h"

namespace yk {
	type_desc::type_desc(position const& pos)
		: ast_node(pos) {
	}

	type_desc::~type_desc() { }

	// Type identifier
	ident_type_desc::ident_type_desc(token const& tok)
		: type_desc(position::get(tok)), Identifier(tok.value()) {
	}

	ident_type_desc::~ident_type_desc() { }

	// Binary type desc
	bin_type_desc::bin_type_desc(type_desc* l, type_desc* r, token const& o)
		: type_desc(position::interval(l->Position, r->Position)), LHS(l), RHS(r), OP(o) {
	}

	bin_type_desc::~bin_type_desc() {
		delete LHS;
		delete RHS;
	}

	// List type desc
	list_type_desc::list_type_desc(yvec<type_desc*> const& ls)
		: type_desc(position::interval(ls[0]->Position, ls[ls.size() - 1]->Position)), List(ls) {
	}

	list_type_desc::~list_type_desc() {
		for (auto i : List) delete i;
	}
}