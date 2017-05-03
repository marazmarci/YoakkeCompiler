#pragma once

#include "ast_node.h"
#include "../parse/token.h"
#include "../utility/optional.h"

namespace yk {
	class type_symbol;

	class ty_expr : public ast_node, public visitable<ty_expr> {
	public:
		META_BaseVisitable(ty_expr)

	public:
		type_symbol* SymbolForm;

	protected:
		ty_expr(position const& pos);

	public:
		virtual ~ty_expr();
	};

	class ident_ty_expr : public ty_expr {
	public:
		META_Visitable(ident_ty_expr, ty_expr)

	public:
		ystr identifier;

	public:
		ident_ty_expr(token const& tok);
		virtual ~ident_ty_expr();
	};

	class bin_ty_expr : public ty_expr {
	public:
		META_Visitable(bin_ty_expr, ty_expr)

	public:
		ty_expr* LHS;
		ty_expr* RHS;
		token OP;

	public:
		bin_ty_expr(ty_expr* l, ty_expr* r, token const& o);
		virtual ~bin_ty_expr();
	};

	class list_ty_expr : public ty_expr {
	public:
		META_Visitable(list_ty_expr, ty_expr)

	public:
		yvec<ty_expr*> List;

	public:
		list_ty_expr(yvec<ty_expr*> const& ls);
		virtual ~list_ty_expr();
	};
}