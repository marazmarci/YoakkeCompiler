#include "pattern.h"

namespace yk {
	pattern::pattern(position const& pos)
		: ast_node(pos) {
	}

	pattern::~pattern() { }

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

	// Enclosed pattern
	enclose_pattern::enclose_pattern(pattern* s, token const& beg, token const& end)
		: pattern(position::interval(position::get(beg), position::get(end))),
		Sub(s), Begin(beg), End(end) {
	}

	enclose_pattern::~enclose_pattern() {
		delete Sub;
	}
}