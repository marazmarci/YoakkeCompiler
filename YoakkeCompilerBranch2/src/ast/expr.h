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

	class ury_expr : public expr {
	public:
		token Operator;
		yshared_ptr<expr> Sub;

	protected:
		ury_expr(token const& op, yshared_ptr<expr> sub, interval const& pos)
			: expr(pos), Operator(op), Sub(sub) {
		}

	public:
		virtual ~ury_expr() { }
	};

	class preury_expr : public ury_expr {
	public:
		preury_expr(token const& op, yshared_ptr<expr> sub)
			: ury_expr(op, sub, interval(op.Position, sub->Position)) {
		}

		virtual ~preury_expr() { }
	};

	class postury_expr : public ury_expr {
	public:
		postury_expr(token const& op, yshared_ptr<expr> sub)
			: ury_expr(op, sub, interval(sub->Position, op.Position)) {
		}

		virtual ~postury_expr() { }
	};

	template <std::size_t>
	class bin_expr : public expr {
	public:
		token Operator;
		yshared_ptr<expr> LHS;
		yshared_ptr<expr> RHS;

	public:
		bin_expr(token const& op, yshared_ptr<expr> lhs, yshared_ptr<expr> rhs)
			: expr(interval(lhs->Position, rhs->Position)),
			Operator(op), LHS(lhs), RHS(rhs) {
		}

		virtual ~bin_expr() { }
	};

	using binop_expr		= bin_expr<0>;
	using asgn_expr			= bin_expr<1>;
	using const_asgn_expr	= bin_expr<2>;

	class list_expr : public expr {
	public:
		std::vector<yshared_ptr<expr>> Expressions;

	public:
		list_expr(yshared_ptr<expr>& left);
		virtual ~list_expr();

	public:
		void add(yshared_ptr<expr>& exp);
	};

	class call_expr : public expr {
	public:
		yshared_ptr<expr> Function;
		yshared_ptr<expr> Args;

	public:
		call_expr(yshared_ptr<expr>& fn, yshared_ptr<expr>& args, token const& end);
		virtual ~call_expr();
	};
}