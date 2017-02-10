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

		ast::Expr* GroupParselet::Parse(ExprParser* parser, Token const& sym)
		{
			auto expr = parser->ParseExpr();
			if (parser->Expect(")"))
			{
				return expr;
			}
			return nullptr;
		}
	}
}