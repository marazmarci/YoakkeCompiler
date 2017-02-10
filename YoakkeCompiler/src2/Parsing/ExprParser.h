#pragma once

#include "Parser.h"
#include "../Common.h"
#include "../AST/Expr.h"

namespace yk
{
	namespace parse
	{
		class PrefixParselet;
		class InfixParselet;
		class Expr;

		class ExprParser : public Parser
		{
		private:
			ymap<ystr, PrefixParselet*> m_PrefixParselets;
			ymap<ystr, InfixParselet*> m_InfixParselets;
			ymap<TokenT, PrefixParselet*> m_PrefixParseletsTT;
			ymap<TokenT, InfixParselet*> m_InfixParseletsTT;

		protected:
			ExprParser(Lexer* lexer, yvec<Token>* tokbuf, dbg::Logger* logger, ystr const& fn);

		public:
			void Register(ystr const& sym, PrefixParselet* parselet);
			void Register(ystr const& sym, InfixParselet* parselet);
			void Register(TokenT tt, PrefixParselet* parselet);
			void Register(TokenT tt, InfixParselet* parselet);

			ysize GetPrecedence();
			ast::Expr* ParseExpr();
			ast::Expr* ParseExpr(ysize prec);
		};
	}
}