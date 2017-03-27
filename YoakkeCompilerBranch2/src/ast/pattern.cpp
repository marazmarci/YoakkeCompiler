#include "pattern.h"

namespace yk {
	// Abstract pattern

	pattern::pattern(interval const& pos)
		: node(pos) {
	}

	pattern::~pattern() { }

	// Identifier pattern

	ident_pattern::ident_pattern(token const& tok)
		: pattern(tok.Position), Identifier(tok.Value) {
	}

	ident_pattern::~ident_pattern() { }

	// Binary pattern

	bin_pattern::bin_pattern(token const& op, yshared_ptr<pattern> lhs, yshared_ptr<pattern> rhs)
		: pattern(interval(lhs->Position, rhs->Position)),
		Operator(op), LHS(lhs), RHS(rhs) {
	}

	bin_pattern::~bin_pattern() { }

	// List pattern

	list_pattern::list_pattern(yshared_ptr<pattern>& left)
		: pattern(left->Position) {
		Elements.push_back(left);
	}

	list_pattern::~list_pattern() { }

	void list_pattern::add(yshared_ptr<pattern>& exp) {
		Elements.push_back(exp);
		Position.End = exp->Position.End;
	}
}
