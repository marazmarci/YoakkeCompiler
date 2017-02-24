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

	// Enclosed type desc
	enclose_type_desc::enclose_type_desc(type_desc* s, token const& beg, token const& end)
		: type_desc(position::interval(position::get(beg), position::get(end))), 
		Sub(s), Begin(beg), End(end) {
	}

	enclose_type_desc::~enclose_type_desc() {
		delete Sub;
	}
}