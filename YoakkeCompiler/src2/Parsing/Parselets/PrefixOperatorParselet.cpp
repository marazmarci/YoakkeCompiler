#include "PrefixOperatorParselet.h"
#include "../ExprParser.h"

namespace yk
{
	namespace parse
	{
		PrefixOperatorParselet::PrefixOperatorParselet(ysize prec)
			: Precedence(prec)
		{
		}

		ast::Expr* PrefixOperatorParselet::Parse(ExprParser* parser, Token const& sym)
		{
			auto right = parser->ParseExpr(Precedence);
			if (right)
				return new ast::PrefixExpr(right, sym);
			std::cout << "ERROR: EXPR EXPECTED" << std::endl;
			return nullptr;
		}
	}
}