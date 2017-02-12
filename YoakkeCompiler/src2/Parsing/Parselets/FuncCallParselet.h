#pragma once

#include "InfixParselet.h"

namespace yk
{
	namespace parse
	{
		class FuncCallParselet : public InfixParselet
		{
		public:
			FuncCallParselet(ysize prec);

		public:
			ast::Expr* Parse(ExprParser* parser, ast::Expr* left, Token const& sym) override;
		};
	}
}