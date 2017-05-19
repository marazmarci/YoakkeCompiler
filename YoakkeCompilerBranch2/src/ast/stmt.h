#pragma once

#include "node.h"
#include "../lexing/token.h"

#define make_stmt(x, ...) make_node(x, stmt, __VA_ARGS__)

namespace yk {
	struct stmt;
	struct expr;

	make_stmt(expr, expr, bool, bool);

	using stmt_ty = yvar<
		  expr_stmt
	>;

	struct stmt {
	public:
		interval	Position;
		stmt_ty		Data;

	public:
		template <typename... Ts>
		stmt(interval const& pos, Ts&&... xs)
			: Position(pos),
			Data(std::forward<Ts>(xs)...) {
		}
	};
}
