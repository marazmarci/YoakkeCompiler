#include "ExprParser.h"
#include "PrefixParselet.h"
#include "InfixParselet.h"
#include "../Utility/MapExtension.h"

namespace yk
{
	namespace parse
	{
		ExprParser::ExprParser(Lexer& lexer)
			: m_Lexer(lexer)
		{
		}

		void ExprParser::Register(ystr const& sym, PrefixParselet* parselet)
		{
			m_Lexer.RegisterSymbol(sym);
			m_PrefixParselets.insert(std::make_pair(sym, parselet));
		}

		void ExprParser::Register(ystr const& sym, InfixParselet* parselet)
		{
			m_Lexer.RegisterSymbol(sym);
			m_InfixParselets.insert(std::make_pair(sym, parselet));
		}

		void ExprParser::Register(TokenT tt, PrefixParselet* parselet)
		{
			m_PrefixParseletsTT.insert(std::make_pair(tt, parselet));
		}

		void ExprParser::Register(TokenT tt, InfixParselet* parselet)
		{
			m_InfixParseletsTT.insert(std::make_pair(tt, parselet));
		}

		ysize ExprParser::GetPrecedence()
		{
			auto parser = m_InfixParselets.find(Peek(0).Value);
			if (parser == m_InfixParselets.end())
				return 0;

			return parser->second->Precedence;
		}

		ast::Expr* ExprParser::ParseExpr()
		{
			return ParseExpr(0);
		}

		ast::Expr* ExprParser::ParseExpr(ysize prec)
		{
			auto lookahead = Consume();
			PrefixParselet* pre_parselet = nullptr;
			if (!(pre_parselet = ext::GetValue(m_PrefixParselets, lookahead.Value))
				&& !(pre_parselet = ext::GetValue(m_PrefixParseletsTT, lookahead.Type)))
			{
				return nullptr;
			}
			auto left = pre_parselet->Parse(this, lookahead);
			while (prec < GetPrecedence())
			{
				lookahead = Consume();
				InfixParselet* in_parselet = nullptr;
				if (!(in_parselet = ext::GetValue(m_InfixParselets, lookahead.Value))
					&& !(in_parselet = ext::GetValue(m_InfixParseletsTT, lookahead.Type)))
				{
					std::cout << "ERROR2" << std::endl;
					return nullptr;
				}
				left = in_parselet->Parse(this, left, lookahead);
			}
			return left;
		}
	}
}