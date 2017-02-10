#include "InfixOperatorParselet.h"
#include "../ExprParser.h"

namespace yk
{
	namespace parse
	{
		InfixOperatorParselet::InfixOperatorParselet(ysize prec, bool r)
			: InfixParselet(prec), Right(r)
		{
		}

		ast::Expr* InfixOperatorParselet::Parse(ExprParser* parser, ast::Expr* left, Token const& sym)
		{
			auto right = parser->ParseExpr(Precedence - (Right ? 1 : 0));
			if (right)
				return new ast::BinExpr(left, right, sym);
			
			std::cout << "ERROR: RHS expected" << std::endl;
			return nullptr;
		}
	}
}