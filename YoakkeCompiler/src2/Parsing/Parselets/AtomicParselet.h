#pragma once

#include "PrefixExprParselet.h"

namespace yk
{
	namespace parse
	{
		class IdentParselet : public PrefixExprParselet
		{
		public:
			ast::Expr* Parse(ExprParser* parser, Token const& sym) override;
		};
	}
}