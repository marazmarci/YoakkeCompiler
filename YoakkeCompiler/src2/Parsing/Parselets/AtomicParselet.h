#pragma once

#include "PrefixParselet.h"

namespace yk
{
	namespace parse
	{
		class IdentParselet : public PrefixParselet
		{
		public:
			ast::Expr* Parse(ExprParser* parser, Token const& sym) override;
		};

		class GroupParselet : public PrefixParselet
		{
		public:
			ast::Expr* Parse(ExprParser* parser, Token const& sym) override;
		};
	}
}