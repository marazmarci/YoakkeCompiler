#pragma once

#include "node.h"
#include "../lexing/token.h"

namespace yk {
	class pat_expr : public node {
	protected:
		pat_expr(interval const& pos);

	public:
		virtual ~pat_expr();
	};

	class ignore_pat_expr : public pat_expr {
	public:
		ignore_pat_expr(token const& tok);
		virtual ~ignore_pat_expr();
	};

	class unit_pat_expr : public pat_expr {
	public:
		unit_pat_expr(token const& beg, token const& end);
		virtual ~unit_pat_expr();
	};

	class ident_pat_expr : public pat_expr {
	public:
		ystr Identifier;

	public:
		ident_pat_expr(token const& tok);
		virtual ~ident_pat_expr();
	};

	class bin_pat_expr : public pat_expr {
	public:
		token Operator;
		ysptr<pat_expr> LHS;
		ysptr<pat_expr> RHS;

	public:
		bin_pat_expr(token const& op, ysptr<pat_expr> lhs, ysptr<pat_expr> rhs);
		virtual ~bin_pat_expr();
	};

	class list_pat_expr : public pat_expr {
	public:
		yvec<ysptr<pat_expr>> Elements;

	public:
		list_pat_expr(ysptr<pat_expr>& left);
		virtual ~list_pat_expr();

	public:
		void add(ysptr<pat_expr>& exp);
	};
}
