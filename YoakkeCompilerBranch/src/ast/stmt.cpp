#include "stmt.h"
#include "expr.h"

namespace yk {
	// Statement
	stmt::stmt(point const& pos)
		: ast_node(pos) {
	}

	stmt::~stmt() { }

	// Expression statement
	expr_stmt::expr_stmt(expr* sub)
		: stmt(sub->Position), Sub(sub), Semicol(None) {
	}

	expr_stmt::expr_stmt(expr* sub, token const& sc)
		: stmt(point::interval(sub->Position, point::get(sc))), Sub(sub), Semicol(sc) {
	}

	expr_stmt::~expr_stmt() {
		delete Sub;
	}
}