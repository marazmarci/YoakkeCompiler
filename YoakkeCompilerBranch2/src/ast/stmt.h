#pragma once

#include "../utility/type_tag.h"
#include "../lexing/token.h"

#define make_stmt(x, ...) make_tag(x, stmt, __VA_ARGS__)

namespace yk {
	struct stmt;
	struct expr;

	//              sub        semicol return
	make_stmt(expr, ysptr<expr>, bool, bool);

	struct stmt {
	public:
		interval	Position;
		yvar<
			ysptr<expr_stmt>
		> Data;

	public:
		template <typename... Ts>
		stmt(interval const& pos, Ts&&... xs)
			: Position(pos),
			Data(std::forward<Ts>(xs)...) {
		}
	};
}
