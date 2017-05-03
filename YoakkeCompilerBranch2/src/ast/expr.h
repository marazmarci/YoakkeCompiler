#pragma once

#include "node.h"
#include "../common.h"
#include "../lexing/token.h"

namespace yk {
	class scope;
	class stmt;
	class pat_expr;
	class ty_expr;
	class type_symbol;

	class expr : public node {
	public:
		ysptr<type_symbol> HintType;
		interval HintPosition;

	protected:
		expr(interval const& pos);

	public:
		virtual ~expr();
	};

	class braced_expr {
	public:
		bool ReturnDestination;

	public:
		braced_expr();
		virtual ~braced_expr();
	};

	class ident_expr : public expr {
	public:
		ystr Identifier;

	public:
		ident_expr(token const& tok);
		virtual ~ident_expr();
	};

	class unit_expr : public expr {
	public:
		unit_expr(token const& beg, token const& end);
		virtual ~unit_expr();
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
		ysptr<expr> Sub;

	protected:
		ury_expr(token const& op, ysptr<expr> sub, interval const& pos)
			: expr(pos), Operator(op), Sub(sub) {
		}

	public:
		virtual ~ury_expr() { }
	};

	class preury_expr : public ury_expr {
	public:
		preury_expr(token const& op, ysptr<expr> sub)
			: ury_expr(op, sub, interval(op.Position, sub->Position)) {
		}

		virtual ~preury_expr() { }
	};

	class postury_expr : public ury_expr {
	public:
		postury_expr(token const& op, ysptr<expr> sub)
			: ury_expr(op, sub, interval(sub->Position, op.Position)) {
		}

		virtual ~postury_expr() { }
	};

	template <ysize>
	class bin_expr : public expr {
	public:
		token Operator;
		ysptr<expr> LHS;
		ysptr<expr> RHS;

	public:
		bin_expr(token const& op, ysptr<expr> lhs, ysptr<expr> rhs)
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
		yvec<ysptr<expr>> Elements;

	public:
		list_expr(ysptr<expr>& left);
		virtual ~list_expr();

	public:
		void add(ysptr<expr>& exp);
	};

	class call_expr : public expr {
	public:
		ysptr<expr> Function;
		ysptr<expr> Args;

	public:
		call_expr(ysptr<expr>& fn, ysptr<expr>& args, token const& end);
		virtual ~call_expr();
	};

	class block_expr : public expr, public braced_expr {
	public:
		yvec<ysptr<stmt>> Statements;
		ysptr<scope> Scope;

	public:
		block_expr(yvec<ysptr<stmt>> sts, token const& beg, token const& end);
		virtual ~block_expr();
	};
	
	class fnproto_expr : public expr {
	public:
		using param_t = ypair<yopt<token>, ysptr<ty_expr>>;

	public:
		yvec<param_t> Parameters;
		ysptr<ty_expr> ReturnType;

	public:
		fnproto_expr(token const& begpar, token const& endpar, yvec<param_t> params, ysptr<ty_expr> ret);
		virtual ~fnproto_expr();
	};

	class fn_expr : public expr, public braced_expr {
	public:
		using param_t = ypair<yopt<token>, ysptr<ty_expr>>;

	public:
		yvec<param_t> Parameters;
		ysptr<ty_expr> ReturnType;
		ysptr<block_expr> Body;

	public:
		fn_expr(token const& begpar, yvec<param_t> params, ysptr<ty_expr> ret, ysptr<block_expr> bd);
		virtual ~fn_expr();
	};

	class let_expr : public expr {
	public:
		ysptr<pat_expr> Pattern;
		ysptr<ty_expr> Type;
		ysptr<expr> Value;

	public:
		let_expr(ysptr<pat_expr> pat, ysptr<ty_expr> ty, ysptr<expr> val, token const& beg);
		virtual ~let_expr();
	};
}
