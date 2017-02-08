#include "ExprParser.h"
#include "PrefixParselet.h"
#include "InfixParselet.h"
#include "../Utility/MapExtension.h"

namespace yk
{
	namespace parse
	{
		ExprParser::ExprParser(const char* buf, ystr const& fn)
		{
			m_Lexer.SetSource(buf, fn);
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

		bool ExprParser::Match(ystr const& expect)
		{
			auto tok = Peek(0);
			if (tok.Value == expect)
			{
				Consume();
				return true;
			}
			return false;
		}

		bool ExprParser::Expect(ystr const& expect)
		{
			auto tok = Peek(0);
			if (tok.Value == expect)
			{
				Consume();
				return true;
			}
			std::cout << "ERROR TOKEN" << std::endl;
			return false;
		}

		Token ExprParser::Consume()
		{
			Peek(0);
			auto tok = m_TokenBuffer[0];
			m_TokenBuffer.erase(m_TokenBuffer.begin());
			return tok;
		}

		Token ExprParser::Peek(ysize delta)
		{
			while (delta >= m_TokenBuffer.size())
			{
				m_TokenBuffer.push_back(m_Lexer.Next());
			}
			return m_TokenBuffer[delta];
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