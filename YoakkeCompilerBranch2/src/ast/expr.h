#pragma once

/*
Here are all the value-expression nodes of the AST.
*/

#include "node.h"
#include "../common.h"
#include "../lexing/token.h"

namespace yk {
	class pat_expr;
	class stmt;
	class type;
	class ty_expr;

	/*
	Expression base-class, contains type-hint information.
	TODO: Deprecated
	*/
	class expr : public node {
	public:
		ysptr<type> HintType;
		interval	HintPosition;

	protected:
		expr(interval const& pos);

	public:
		virtual ~expr();
	};

	/*
	A braced expression base-class, contains if it needs to capture a value.
	*/
	class braced_expr {
	public:
		bool ReturnDest;

	protected:
		braced_expr();

	public:
		virtual ~braced_expr();
	};

	/*
	Identifier expression.
	*/
	class ident_expr : public expr {
	public:
		ystr Identifier;

	public:
		ident_expr(token const& tok);
		virtual ~ident_expr();
	};

	/*
	A tuple without elements.
	*/
	class unit_expr : public expr {
	public:
		unit_expr(token const& beg, token const& end);
		virtual ~unit_expr();
	};

	/*
	Integer literal.
	*/
	class int_lit_expr : public expr {
	public:
		long long int Value;

	public:
		int_lit_expr(token const& tok);
		virtual ~int_lit_expr();
	};

	/*
	Floating-point literal.
	*/
	class real_lit_expr : public expr {
	public:
		long double Value;

	public:
		real_lit_expr(token const& tok);
		virtual ~real_lit_expr();
	};

	/*
	Base-class for unary expression.
	*/
	class ury_expr : public expr {
	public:
		token		Operator;
		ysptr<expr> Sub;

	protected:
		ury_expr(token const& op, ysptr<expr> sub, interval const& pos);

	public:
		virtual ~ury_expr();
	};

	/*
	Prefix unary expression. (position: op then subexpr)
	*/
	class preury_expr : public ury_expr {
	public:
		preury_expr(token const& op, ysptr<expr> sub);
		virtual ~preury_expr();
	};

	/*
	Postfix unary expression. (position: subexpr then op)
	*/
	class postury_expr : public ury_expr {
	public:
		postury_expr(token const& op, ysptr<expr> sub);
		virtual ~postury_expr();
	};

	/*
	Base-class for binary expressions.
	*/
	class bin_expr : public expr {
	public:
		token		Operator;
		ysptr<expr> LHS;
		ysptr<expr> RHS;

	protected:
		bin_expr(token const& op, ysptr<expr> lhs, ysptr<expr> rhs);

	public:
		virtual ~bin_expr();
	};

	/*
	Binary operation expression.
	*/
	class binop_expr : public bin_expr {
	public:
		binop_expr(token const& op, ysptr<expr> lhs, ysptr<expr> rhs);
		virtual ~binop_expr();
	};

	/*
	Assignment expression.
	*/
	class asgn_expr : public bin_expr {
	public:
		asgn_expr(token const& op, ysptr<expr> lhs, ysptr<expr> rhs);
		virtual ~asgn_expr();
	};

	/*
	Constant assignment expression.
	*/
	class const_asgn_expr : public bin_expr {
	public:
		const_asgn_expr(token const& op, ysptr<expr> lhs, ysptr<expr> rhs);
		virtual ~const_asgn_expr();
	};

	/*
	List or tuple expression.
	*/
	class list_expr : public expr {
	public:
		yvec<ysptr<expr>> Elements;

	public:
		list_expr(ysptr<expr>& left);
		virtual ~list_expr();

	public:
		void add(ysptr<expr>& exp);
	};

	/*
	Function call expression.
	*/
	class call_expr : public expr {
	public:
		ysptr<expr> Function;
		ysptr<expr> Args;

	public:
		call_expr(ysptr<expr>& fn, ysptr<expr>& args, token const& end);
		virtual ~call_expr();
	};

	/*
	Scoped block expression.
	*/
	class block_expr : public expr, public braced_expr {
	public:
		yvec<ysptr<stmt>> Statements;

	public:
		block_expr(yvec<ysptr<stmt>> sts, token const& beg, token const& end);
		virtual ~block_expr();
	};
	
	/*
	Function prototype expression.
	*/
	class fnproto_expr : public expr {
	public:
		using param_t = ypair<yopt<token>, ysptr<ty_expr>>;

	public:
		yvec<param_t>	Parameters;
		ysptr<ty_expr>	ReturnType;

	public:
		fnproto_expr(token const& begpar, token const& endpar, yvec<param_t> params, ysptr<ty_expr> ret);
		virtual ~fnproto_expr();
	};

	/*
	Function expression.
	*/
	class fn_expr : public expr, public braced_expr {
	public:
		using param_t = ypair<yopt<token>, ysptr<ty_expr>>;

	public:
		yvec<param_t>		Parameters;
		ysptr<ty_expr>		ReturnType;
		ysptr<block_expr>	Body;

	public:
		fn_expr(token const& begpar, yvec<param_t> params, ysptr<ty_expr> ret, ysptr<block_expr> bd);
		virtual ~fn_expr();
	};

	/*
	Let variable binding expression.
	*/
	class let_expr : public expr {
	public:
		ysptr<pat_expr>	Pattern;
		ysptr<ty_expr>	Type;
		ysptr<expr>		Value;

	public:
		let_expr(ysptr<pat_expr> pat, ysptr<ty_expr> ty, ysptr<expr> val, token const& beg);
		virtual ~let_expr();
	};
}
