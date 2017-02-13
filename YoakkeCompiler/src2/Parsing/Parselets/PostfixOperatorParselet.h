#pragma once

#include "InfixParselet.h"

namespace yk
{
	namespace parse
	{
		class PostfixOperatorParselet : public InfixExprParselet
		{
		public:
			PostfixOperatorParselet(ysize prec);

		public:
			ast::Expr* Parse(ExprParser* parser, ast::Expr* left, Token const& sym) override;
		};
	}
}