#include "pattern.h"

namespace yk {
	pat_expr::pat_expr(point const& pos)
		: ast_node(pos) {
	}

	pat_expr::~pat_expr() { }

	// Skip pattern
	skip_pattern::skip_pattern(token const& tok)
		: pat_expr(point::get(tok)) {
	}

	skip_pattern::~skip_pattern() { }

	// Ident pattern
	ident_pat_expr::ident_pat_expr(token const& tok)
		: pat_expr(point::get(tok)), identifier(tok.value()) {
	}

	ident_pat_expr::~ident_pat_expr() { }

	// Binary pattern
	bin_pat_expr::bin_pat_expr(pat_expr* l, pat_expr* r, token const& o)
		: pat_expr(point::interval(l->Position, r->Position)), LHS(l), RHS(r), OP(o) {
	}

	bin_pat_expr::~bin_pat_expr() {
		delete LHS;
		delete RHS;
	}

	// List pattern
	list_pat_expr::list_pat_expr(yvec<pat_expr*> const& ls)
		: pat_expr(point::interval(ls[0]->Position, ls[ls.size() - 1]->Position)), List(ls) {
	}

	list_pat_expr::~list_pat_expr() {
		for (auto i : List) delete i;
	}
}