#pragma once

#include "ast_node.h"

namespace yk {
	class pat_expr : public ast_node {
	protected:
		pat_expr(point const& pos);

	public:
		virtual ~pat_expr();
	};

	class skip_pattern : public pat_expr {
	public:
		skip_pattern(token const& tok);
		virtual ~skip_pattern();
	};

	class ident_pat_expr : public pat_expr {
	public:
		ystr identifier;

	public:
		ident_pat_expr(token const& tok);
		virtual ~ident_pat_expr();
	};

	class bin_pat_expr : public pat_expr {
	public:
		pat_expr* LHS;
		pat_expr* RHS;
		token OP;

	public:
		bin_pat_expr(pat_expr* l, pat_expr* r, token const& o);
		virtual ~bin_pat_expr();
	};

	class list_pat_expr : public pat_expr {
	public:
		yvec<pat_expr*> List;

	public:
		list_pat_expr(yvec<pat_expr*> const& ls);
		virtual ~list_pat_expr();
	};
}