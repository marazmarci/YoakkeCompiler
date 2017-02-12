#include "FuncCallParselet.h"
#include "../ExprParser.h"

namespace yk
{
	namespace parse
	{
		FuncCallParselet::FuncCallParselet(ysize prec)
			: InfixParselet(prec)
		{
		}

		ast::Expr* FuncCallParselet::Parse(ExprParser* parser, ast::Expr* left, Token const& sym)
		{
			yvec<ast::Expr*> operands;
			operands.push_back(left);
			auto last = parser->Peek(0);

			if (!parser->Match(")"))
			{
				do
				{
					auto arg = parser->ParseExpr();
					if (!arg)
					{
						std::cout << "ERROR arg" << std::endl;
						return nullptr;
					}
					operands.push_back(arg);
				} while (parser->Match(","));
				last = parser->Peek(0);
				parser->Expect(")");
			}

			return new ast::MixfixExpr("call", operands, 
				parse::Position::Interval(left->Pos, parse::Position::Get(last)));
		}
	}
}