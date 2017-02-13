#include "AtomicParselet.h"
#include "../ExprParser.h"

namespace yk
{
	namespace parse
	{
		ast::Expr* IdentParselet::Parse(ExprParser* parser, Token const& sym)
		{
			return new ast::IdentExpr(sym);
		}
	}
}