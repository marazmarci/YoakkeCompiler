#include "ast_node.h"

namespace yk {
	ast_node::ast_node(position const& pos)
		: Position(pos) {
	}

	ast_node::~ast_node() { }
}