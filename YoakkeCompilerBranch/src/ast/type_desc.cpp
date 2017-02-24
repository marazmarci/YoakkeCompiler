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
}