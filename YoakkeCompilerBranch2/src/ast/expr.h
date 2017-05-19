#pragma once

#include "node.h"
#include "../lexing/token.h"

#define make_expr(x, ...) make_node(x, expr, __VA_ARGS__)

namespace yk {
	struct stmt;
	struct ty_expr;
	struct pat_expr;
	struct expr;
	struct type;

	using param_t = ypair<yopt<token>, ty_expr>;

	make_expr(ident,		ystr);
	make_expr(unit			);
	make_expr(int_lit,		int);
	make_expr(real_lit,		double);
	make_expr(preury,		token, expr);
	make_expr(postury,		token, expr);
	make_expr(binop,		token, expr, expr);
	make_expr(asgn,			token, expr, expr);
	make_expr(const_asgn,	token, expr, expr);
	make_expr(list,			yvec<expr>);
	make_expr(call,			expr, yopt<expr>);
	make_expr(block, 		yvec<stmt>, bool);
	make_expr(fnproto,		yvec<param_t>, ty_expr);
	make_expr(fn,			yvec<param_t>, yopt<ty_expr>, block_expr);
	make_expr(let,			pat_expr, yopt<ty_expr>, yopt<expr>);

	using expr_ty = yvar<
		  ysptr<ident_expr>
		, ysptr<unit_expr>
		, ysptr<int_lit_expr>
		, ysptr<real_lit_expr>
		, ysptr<preury_expr>
		, ysptr<postury_expr>
		, ysptr<binop_expr>
		, ysptr<asgn_expr>
		, ysptr<const_asgn_expr>
		, ysptr<list_expr>
		, ysptr<call_expr>
		, ysptr<block_expr>
		, ysptr<fnproto_expr>
		, ysptr<fn_expr>
	>;

	struct expr {
	public:
		interval	Position;
		expr_ty		Data;
		ysptr<type> HintType;
		interval	HintPosition;

	public:
		template <typename... Ts>
		expr(interval const& pos, Ts&&... xs)
			: Position(pos),
			Data(std::forward<Ts>(xs)...),
			HintType(nullptr),
			HintPosition(interval()) {
		}
	};
}
