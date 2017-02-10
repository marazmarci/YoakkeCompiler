#include "YExprParser.h"
#include "Parselets\ExprParselets.h"

namespace yk
{
	namespace parse
	{
		YExprParser::YExprParser(Lexer* lexer, yvec<Token>* tokbuf, dbg::Logger* logger, ystr const& fn)
			: ExprParser(lexer, tokbuf, logger, fn)
		{
			m_Lexer->RegisterSymbol("(");
			m_Lexer->RegisterSymbol(")");

			Register(TokenT::Identifier,	new IdentParselet());
			Register("(",					new GroupParselet());

			AddInfixLeft("+", 1);
			AddInfixLeft("-", 1);
			AddInfixLeft("*", 2);
			AddInfixLeft("/", 2);
		}

		void YExprParser::AddPostfix(ystr const& op, ysize prec)
		{
			Register(op, new PostfixOperatorParselet(prec));
		}

		void YExprParser::AddPrefix(ystr const& op, ysize prec)
		{
			Register(op, new PrefixOperatorParselet(prec));
		}

		void YExprParser::AddInfixLeft(ystr const& op, ysize prec)
		{
			Register(op, new InfixOperatorParselet(prec, false));
		}

		void YExprParser::AddInfixRight(ystr const& op, ysize prec)
		{
			Register(op, new InfixOperatorParselet(prec, true));
		}
	}
}
