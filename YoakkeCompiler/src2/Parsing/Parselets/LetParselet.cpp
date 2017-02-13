#include "LetParselet.h"
#include "../ExprParser.h"

namespace yk
{
	namespace parse
	{
		LetParselet::LetParselet(ysize ap)
			: AsgnPrec(ap)
		{
		}

		ast::Expr* LetParselet::Parse(ExprParser* parser, Token const& sym)
		{
			auto lhs = parser->ParseExpr(AsgnPrec + 1);
			if (lhs)
			{
				ast::TypeDesc* type = nullptr;
				ast::Expr* val = nullptr;
				if (parser->Match(":"))
				{
					type = parser->ParseTypeDesc();
					if (!type)
					{
						std::cout << "ERROR: TYPE EXPECTED AFTER ':' IN LET" << std::endl;
						return nullptr;
					}
				}
				if (parser->Match("="))
				{
					val = parser->ParseExpr();
					if (!type)
					{
						std::cout << "ERROR: EXPR EXPECTED AFTER '=' IN LET" << std::endl;
						return nullptr;
					}
				}
				return new ast::LetExpr(lhs, val, type);
			}
			else
				std::cout << "ERROR: LET HLS EXPECTED" << std::endl;

			return nullptr;
		}
	}
}