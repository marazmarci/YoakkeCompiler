#include "YExprParser.h"
#include "Parselets\ExprParselets.h"

namespace yk
{
	namespace parse
	{
		YExprParser::YExprParser(Lexer* lexer, yvec<Token>* tokbuf, dbg::Logger* logger, ystr const& fn)
			: ExprParser(lexer, tokbuf, logger, fn)
		{
			Register(TokenT::Identifier,	new IdentParselet());
			AddEnclose(new EncloseParselet("(", ")"));

			AddInfixLeft(",", 0);

			AddInfixRight("=", 1);

			AddInfixLeft("||", 2);
			AddInfixLeft("&&", 3);

			AddInfixLeft("==", 4);
			AddInfixLeft("<>", 4);

			AddInfixLeft("<", 5);
			AddInfixLeft(">", 5);
			AddInfixLeft("<=", 5);
			AddInfixLeft(">=", 5);

			AddInfixLeft("+", 6);
			AddInfixLeft("-", 6);

			AddInfixLeft("*", 7);
			AddInfixLeft("/", 7);
			AddInfixLeft("%", 7);

			AddPrefix("+", 8);
			AddPrefix("-", 8);
			AddPrefix("!", 8);

			AddInfixLeft(".", 9);
			Register("(", new FuncCallParselet(9));

			AddInfixLeft("::", 10);
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

		void YExprParser::AddEnclose(EncloseParselet* enc)
		{
			Register(enc->Left, enc);
		}
	}
}
