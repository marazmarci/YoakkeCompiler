#include "PostfixOperatorParselet.h"

namespace yk
{
	namespace parse
	{
		PostfixOperatorParselet::PostfixOperatorParselet(ysize prec)
			: InfixExprParselet(prec)
		{
		}

		ast::Expr* PostfixOperatorParselet::Parse(ExprParser* parser, ast::Expr* left, Token const& sym)
		{
			return new ast::PostfixExpr(left, sym);
		}
	}
}