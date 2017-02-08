#pragma once

#include "../AST/Expr.h"

namespace yk
{
	namespace parse
	{
		class ExprParser;

		class InfixParselet
		{
		public:
			const ysize Precedence;

		public:
			InfixParselet(ysize prec);

		public:
			virtual ast::Expr* Parse(ExprParser* parser, ast::Expr* left, Token const& sym) = 0;
		};
	}
}