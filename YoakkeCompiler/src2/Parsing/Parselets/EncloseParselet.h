#pragma once

#include "PrefixParselet.h"

namespace yk
{
	namespace parse
	{
		class EncloseParselet : public PrefixParselet
		{
		public:
			ystr Left;
			ystr Right;

		public:
			EncloseParselet(ystr const& l, ystr const& r);

		public:
			ast::Expr* Parse(ExprParser* parser, Token const& sym) override;
		};
	}
}