#include "ast.h"

namespace yk {
	// Abstract statement class

	stmt::stmt(interval const& pos)
		: node(pos) {
	}

	stmt::~stmt() { }

	// Expression statement

	expr_stmt::expr_stmt(ysptr<expr>& exp, yopt<token> semicol)
		: stmt(interval(exp->Position, 
			semicol.has_value() ? semicol.value().Position : exp->Position)),
			Expression(exp), Semicol(semicol.has_value()),
			Return(false) {
	}

	expr_stmt::~expr_stmt() { }
}
