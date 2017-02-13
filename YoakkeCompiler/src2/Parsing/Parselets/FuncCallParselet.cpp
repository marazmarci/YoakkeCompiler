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
				auto params = parser->ParseExpr();
				if (!params)
				{
					std::cout << "ERROR: args expeced" << std::endl;
					return nullptr;
				}
				operands.push_back(params);
				last = parser->Peek(0);
				parser->Expect(")");
			}

			return new ast::MixfixExpr("call", operands, 
				parse::Position::Interval(left->Pos, parse::Position::Get(last)));
		}
	}
}