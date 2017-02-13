#pragma once

#include "IPrefixParselet.h"
#include "../../AST/Expr.h"

namespace yk
{
	namespace parse
	{
		class ExprParser;

		typedef IPrefixParselet<ast::Expr, ExprParser> PrefixExprParselet;
		typedef IPrefixParselet<ast::TypeDesc, ExprParser> PrefixTypeDescParselet;
	}
}