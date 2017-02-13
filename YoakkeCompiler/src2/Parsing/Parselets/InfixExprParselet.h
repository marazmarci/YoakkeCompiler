#pragma once

#include "IInfixParselet.h"
#include "../../AST/Expr.h"

namespace yk
{
	namespace parse
	{
		class ExprParser;

		typedef IInfixParselet<ast::Expr, ExprParser> InfixExprParselet;
		typedef IInfixParselet<ast::TypeDesc, ExprParser> InfixTypeDescParselet;
	}
}