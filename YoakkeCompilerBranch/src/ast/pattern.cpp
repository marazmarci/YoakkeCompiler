#include "pattern.h"

namespace yk {
	pattern::pattern(position const& pos)
		: ast_node(pos) {
	}

	pattern::~pattern() { }

	// Skip pattern
	skip_pattern::skip_pattern(token const& tok)
		: pattern(position::get(tok)) {
	}

	skip_pattern::~skip_pattern() { }

	// Ident pattern
	ident_pattern::ident_pattern(token const& tok)
		: pattern(position::get(tok)), Identifier(tok.value()) {
	}

	ident_pattern::~ident_pattern() { }

	// Binary pattern
	bin_pattern::bin_pattern(pattern* l, pattern* r, token const& o)
		: pattern(position::interval(l->Position, r->Position)), LHS(l), RHS(r), OP(o) {
	}

	bin_pattern::~bin_pattern() {
		delete LHS;
		delete RHS;
	}

	// List pattern
	list_pattern::list_pattern(yvec<pattern*> const& ls)
		: pattern(position::interval(ls[0]->Position, ls[ls.size() - 1]->Position)), List(ls) {
	}

	list_pattern::~list_pattern() {
		for (auto i : List) delete i;
	}
}