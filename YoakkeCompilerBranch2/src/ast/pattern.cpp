#include "pattern.h"

namespace yk {
	// Abstract pattern

	pattern::pattern(interval const& pos)
		: node(pos) {
	}

	pattern::~pattern() { }

	// Ignore pattern

	ignore_pattern::ignore_pattern(token const& tok)
		: pattern(tok.Position) {
	}

	ignore_pattern::~ignore_pattern() { }

	// Unit pattern

	unit_pattern::unit_pattern(token const& beg, token const& end)
		: pattern(interval(beg.Position, end.Position)) {
	}

	unit_pattern::~unit_pattern() { }

	// Identifier pattern

	ident_pattern::ident_pattern(token const& tok)
		: pattern(tok.Position), Identifier(tok.Value) {
	}

	ident_pattern::~ident_pattern() { }

	// Binary pattern

	bin_pattern::bin_pattern(token const& op, ysptr<pattern> lhs, ysptr<pattern> rhs)
		: pattern(interval(lhs->Position, rhs->Position)),
		Operator(op), LHS(lhs), RHS(rhs) {
	}

	bin_pattern::~bin_pattern() { }

	// List pattern

	list_pattern::list_pattern(ysptr<pattern>& left)
		: pattern(left->Position) {
		Elements.push_back(left);
	}

	list_pattern::~list_pattern() { }

	void list_pattern::add(ysptr<pattern>& exp) {
		Elements.push_back(exp);
		Position.End = exp->Position.End;
	}
}
