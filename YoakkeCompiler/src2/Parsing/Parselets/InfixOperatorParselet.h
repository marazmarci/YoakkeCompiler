#pragma once

#include "InfixParselet.h"

namespace yk
{
	namespace parse
	{
		class InfixOperatorParselet : public InfixExprParselet
		{
		public:
			bool Right;

		public:
			InfixOperatorParselet(ysize prec, bool r);

		public:
			ast::Expr* Parse(ExprParser* parser, ast::Expr* left, Token const& sym) override;
		};
	}
}