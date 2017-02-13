#pragma once

#include "PrefixParselet.h"

namespace yk
{
	namespace parse
	{
		class LetParselet : public PrefixExprParselet
		{
		public:
			ysize AsgnPrec;

		public:
			LetParselet(ysize ap);

		public:
			ast::Expr* Parse(ExprParser* parser, Token const& sym) override;
		};
	}
}