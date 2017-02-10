#pragma once

#include "../../AST/Expr.h"

namespace yk
{
	namespace parse
	{
		class ExprParser;

		class PrefixParselet
		{
		public:
			virtual ast::Expr* Parse(ExprParser* parser, Token const& sym) = 0;
		};
	}
}