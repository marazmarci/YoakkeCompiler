#pragma once

#include "PrefixParselet.h"

namespace yk
{
	namespace parse
	{
		class PrefixOperatorParselet : public PrefixParselet
		{
		public:
			ysize Precedence;

		public:
			PrefixOperatorParselet(ysize prec);

		public:
			ast::Expr* Parse(ExprParser* parser, Token const& sym) override;
		};
	}
}