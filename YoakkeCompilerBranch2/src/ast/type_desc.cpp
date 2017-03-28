#include "type_desc.h"

namespace yk {
	// Abstract type descriptor

	type_desc::type_desc(interval const& pos)
		: node(pos) {
	}

	type_desc::~type_desc() { }

	// Identifier type descriptor

	ident_type_desc::ident_type_desc(token const& tok)
		: type_desc(tok.Position), Identifier(tok.Value) {
	}

	ident_type_desc::~ident_type_desc() { }

	// Binary type descriptor

	bin_type_desc::bin_type_desc(token const& op, yshared_ptr<type_desc> lhs, yshared_ptr<type_desc> rhs)
		: type_desc(interval(lhs->Position, rhs->Position)),
		Operator(op), LHS(lhs), RHS(rhs) {
	}

	bin_type_desc::~bin_type_desc() { }

	// List type descriptor

	list_type_desc::list_type_desc(yshared_ptr<type_desc>& left)
		: type_desc(left->Position) {
		Elements.push_back(left);
	}

	list_type_desc::~list_type_desc() { }

	void list_type_desc::add(yshared_ptr<type_desc>& exp) {
		Elements.push_back(exp);
		Position.End = exp->Position.End;
	}
}
