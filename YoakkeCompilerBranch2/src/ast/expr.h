#pragma once

#include "../utility/type_tag.h"
#include "../lexing/token.h"
#include "../semantics/type.h"

#define make_expr(x, ...) make_tag(x, expr, __VA_ARGS__)

namespace yk {
	struct stmt;
	struct ty_expr;
	struct pat_expr;
	struct expr;
	struct type;
	class scope;

	using param_t = ypair<yopt<token>, ty_expr>;

	make_expr(ident,		ystr);
	make_expr(unit			);
	make_expr(int_lit,		ystr);
	make_expr(real_lit,		ystr);
	make_expr(preury,		token, expr);
	make_expr(postury,		token, expr);
	make_expr(binop,		token, expr, expr);
	make_expr(asgn,			token, expr, expr);
	make_expr(const_asgn,	token, expr, expr);
	make_expr(list,			yvec<expr>);
	make_expr(call,			expr, yopt<expr>);
	make_expr(block, 		yvec<stmt>, bool, ysptr<scope>);
	make_expr(fnproto,		yvec<param_t>, ty_expr);
	make_expr(fn,			yvec<param_t>, yopt<ty_expr>, expr);
	make_expr(let,			pat_expr, yopt<ty_expr>, yopt<expr>);

	struct expr {
	public:
		interval	Position;
		yvar<
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
			, ysptr<let_expr>
		> Data;
		yopt<type>	HintType;
		interval	HintPosition;

	public:
		template <typename... Ts>
		expr(interval const& pos, Ts&&... xs)
			: Position(pos),
			Data(std::forward<Ts>(xs)...),
			HintType(yopt<type>{}),
			HintPosition(interval()) {
		}
	};
}
