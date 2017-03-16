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
		~ident_expr();
	};
}
