#include "expr.h"

namespace yk {
	expr::expr(position const& pos)
		: ast_node(pos) {
	}

	expr::~expr() { }
}