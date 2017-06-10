#include "type_desc.h"

namespace yk {
	ty_expr::ty_expr(point const& pos)
		: ast_node(pos), SymbolForm(nullptr) {
	}

	ty_expr::~ty_expr() { }

	// Type identifier
	ident_ty_expr::ident_ty_expr(token const& tok)
		: ty_expr(point::get(tok)), identifier(tok.value()) {
	}

	ident_ty_expr::~ident_ty_expr() { }

	// Binary type desc
	bin_ty_expr::bin_ty_expr(ty_expr* l, ty_expr* r, token const& o)
		: ty_expr(point::interval(l->Position, r->Position)), LHS(l), RHS(r), OP(o) {
	}

	bin_ty_expr::~bin_ty_expr() {
		delete LHS;
		delete RHS;
	}

	// List type desc
	list_ty_expr::list_ty_expr(yvec<ty_expr*> const& ls)
		: ty_expr(point::interval(ls[0]->Position, ls[ls.size() - 1]->Position)), List(ls) {
	}

	list_ty_expr::~list_ty_expr() {
		for (auto i : List) delete i;
	}
}