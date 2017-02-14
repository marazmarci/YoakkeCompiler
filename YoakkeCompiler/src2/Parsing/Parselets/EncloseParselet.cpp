#include "EncloseParselet.h"
#include "../ExprParser.h"

namespace yk
{
	namespace parse
	{
		EncloseParselet::EncloseParselet(ystr const& l, ystr const& r)
			: Left(l), Right(r)
		{
		}

		ast::Expr* EncloseParselet::Parse(ExprParser* parser, Token const& sym)
		{
			auto expr = parser->ParseExpr();
			auto rig = parser->Peek(0);
			if (parser->Expect(Right))
			{
				return new ast::EnclosedExpr(sym, rig, expr);
			}
			else
			{
				std::cout << "ERROR, enclosed needs expression inside" << std::endl;
			}
			return nullptr;
		}
	}
}