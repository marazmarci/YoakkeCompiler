#pragma once

#include "Lexer.h"
#include "Token.h"
#include "../Common.h"
#include "../AST/Expr.h"

namespace yk
{
	namespace parse
	{
		class PrefixParselet;
		class InfixParselet;
		class Expr;

		class ExprParser
		{
		protected:
			Lexer m_Lexer;

		private:
			ymap<ystr, PrefixParselet*> m_PrefixParselets;
			ymap<ystr, InfixParselet*> m_InfixParselets;
			ymap<TokenT, PrefixParselet*> m_PrefixParseletsTT;
			ymap<TokenT, InfixParselet*> m_InfixParseletsTT;
			yvec<Token> m_TokenBuffer;

		public:
			ExprParser(const char* buf, ystr const& fn);

		public:
			void Register(ystr const& sym, PrefixParselet* parselet);
			void Register(ystr const& sym, InfixParselet* parselet);
			void Register(TokenT tt, PrefixParselet* parselet);
			void Register(TokenT tt, InfixParselet* parselet);

		public:
			bool Match(ystr const& expect);
			bool Expect(ystr const& expect);
			Token Consume();
			Token Peek(ysize delta);

			ysize GetPrecedence();

			ast::Expr* ParseExpr();
			ast::Expr* ParseExpr(ysize prec);
		};
	}
}