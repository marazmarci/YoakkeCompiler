#pragma once

#include "node.h"
#include "../lexing/token.h"

#define make_pat(x, ...) make_node(x, pat_expr, __VA_ARGS__)

namespace yk {
	struct pat_expr;

	make_pat(ignore	);
	make_pat(unit	);
	make_pat(ident,	ystr);
	make_pat(list,	yvec<pat_expr>);

	using pat_ty = yvar<
		  ignore_pat_expr
		, unit_pat_expr
		, ident_pat_expr
		, list_pat_expr
	>;

	struct pat_expr {
	public:
		interval	Position;
		pat_ty		Data;

	public:
		template <typename... Ts>
		pat_expr(interval const& pos, Ts&&... xs)
			: Position(pos),
			Data(std::forward<Ts>(xs)...) {
		}
	};
}
