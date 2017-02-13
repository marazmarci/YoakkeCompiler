#pragma once

#include "PrefixParselet.h"

namespace yk
{
	namespace parse
	{
		class ExprParser;

		template <typename T, typename U>
		class IdentParselet : public U
		{
		public:
			T* Parse(ExprParser* parser, Token const& sym) override
			{
				return new T(sym);
			}
		};

		typedef IdentParselet<ast::IdentExpr, PrefixExprParselet> IdentExprParselet;
		typedef IdentParselet<ast::IdentTypeDesc, PrefixTypeDescParselet> IdentTypeParselet;
	}
}