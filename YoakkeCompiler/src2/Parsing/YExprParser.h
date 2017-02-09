#pragma once

#include "ExprParser.h"

namespace yk
{
	namespace parse
	{
		class YExprParser : public ExprParser
		{
		public:
			YExprParser(Lexer& lexer);

		private:
			void AddPostfix(ystr const& op, ysize prec);
			void AddPrefix(ystr const& op, ysize prec);
			void AddInfixLeft(ystr const& op, ysize prec);
			void AddInfixRight(ystr const& op, ysize prec);
		};
	}
}