#pragma once

#include "node.h"
#include "../lexing/token.h"

namespace yk {
	/*
	Base-class of type expressions.
	*/
	class ty_expr : public node {
	protected:
		ty_expr(interval const& pos);

	public:
		virtual ~ty_expr();
	};

	/*
	Unit or empty tuple type.
	*/
	class unit_ty_expr : public ty_expr {
	public:
		unit_ty_expr(token const& beg, token const& end);
		virtual ~unit_ty_expr();
	};

	/*
	Identifier type.
	*/
	class ident_ty_expr : public ty_expr {
	public:
		ystr Identifier;

	public:
		ident_ty_expr(token const& tok);
		virtual ~ident_ty_expr();
	};

	/*
	Binary type construct.
	*/
	class bin_ty_expr : public ty_expr {
	public:
		token Operator;
		ysptr<ty_expr> LHS;
		ysptr<ty_expr> RHS;

	public:
		bin_ty_expr(token const& op, ysptr<ty_expr> lhs, ysptr<ty_expr> rhs);
		virtual ~bin_ty_expr();
	};

	/*
	List or tuple type.
	*/
	class list_ty_expr : public ty_expr {
	public:
		yvec<ysptr<ty_expr>> Elements;

	public:
		list_ty_expr(ysptr<ty_expr>& left);
		virtual ~list_ty_expr();

	public:
		void add(ysptr<ty_expr>& exp);
	};
}
