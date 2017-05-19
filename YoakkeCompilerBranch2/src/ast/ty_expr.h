#pragma once

#include "node.h"
#include "../lexing/token.h"

#define make_ty(x, ...) make_node(x, ty_expr, __VA_ARGS__)

namespace yk {
	struct ty_expr;

	make_ty(unit	);
	make_ty(ident,	ystr);
	make_ty(bin,	token, ty_expr, ty_expr);
	make_ty(list,	yvec<ty_expr>);

	using ty_ty = yvar<
		  unit_ty_expr
		, ident_ty_expr
		, bin_ty_expr
		, list_ty_expr
	>;

	struct ty_expr {
	public:
		interval	Position;
		ty_ty		Data;

	public:
		template <typename... Ts>
		ty_expr(interval const& pos, Ts&&... xs)
			: Position(pos),
			Data(std::forward<Ts>(xs)...) {
		}
	};
}
