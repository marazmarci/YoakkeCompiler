#include "type_desc.h"

namespace yk {
	type_desc::type_desc(position const& pos)
		: ast_node(pos) {
	}

	type_desc::~type_desc() { }
}