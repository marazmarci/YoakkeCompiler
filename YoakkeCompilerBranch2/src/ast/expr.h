#pragma once

#include "node.h"
#include "../common.h"
#include "../lexing/token.h"

namespace yk {
	class expr : public node {
	protected:
		expr(interval const& pos);

	public:
		virtual ~expr();
	};

	class ident_expr : public expr {
	public:
		ystr Identifier;

	public:
		ident_expr(token const& tok);
		virtual ~ident_expr();
	};

	class int_lit_expr : public expr {
	public:
		long long int Value;

	public:
		int_lit_expr(token const& tok);
		virtual ~int_lit_expr();
	};

	class real_lit_expr : public expr {
	public:
		long double Value;

	public:
		real_lit_expr(token const& tok);
		virtual ~real_lit_expr();
	};

	class preury_expr : public expr {
	public:
		token Operator;
		yshared_ptr<expr> Sub;

	public:
		preury_expr(token const& op, yshared_ptr<expr> sub);
		virtual ~preury_expr();
	};

	class postury_expr : public expr {
	public:
		token Operator;
		yshared_ptr<expr> Sub;

	public:
		postury_expr(token const& op, yshared_ptr<expr> sub);
		virtual ~postury_expr();
	};

	class bin_expr : public expr {
	public:
		token Operator;
		yshared_ptr<expr> LHS;
		yshared_ptr<expr> RHS;

	public:
		bin_expr(token const& op, yshared_ptr<expr> lhs, yshared_ptr<expr> rhs);
		virtual ~bin_expr();
	};
}
