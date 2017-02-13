#include "ExprParser.h"
#include "Parselets/PrefixExprParselet.h"
#include "Parselets/InfixExprParselet.h"
#include "../Utility/MapExtension.h"

namespace yk
{
	namespace parse
	{
		ExprParser::ExprParser(Lexer* lexer, yvec<Token>* tokbuf, dbg::Logger* logger, ystr const& fn)
			: Parser(lexer, tokbuf, logger, fn)
		{
		}

		void ExprParser::Register(ystr const& sym, PrefixExprParselet* parselet)
		{
			m_Lexer->RegisterSymbol(sym);
			auto& map = std::get<1>(m_ExprParselets);
			map.insert(std::make_pair(sym, parselet));
		}

		void ExprParser::Register(ystr const& sym, InfixExprParselet* parselet)
		{
			m_Lexer->RegisterSymbol(sym);
			auto& map = std::get<3>(m_ExprParselets);
			map.insert(std::make_pair(sym, parselet));
		}

		void ExprParser::Register(TokenT tt, PrefixExprParselet* parselet)
		{
			auto& map = std::get<0>(m_ExprParselets);
			map.insert(std::make_pair(tt, parselet));
		}

		void ExprParser::Register(TokenT tt, InfixExprParselet* parselet)
		{
			auto& map = std::get<2>(m_ExprParselets);
			map.insert(std::make_pair(tt, parselet));
		}

		ast::Expr* ExprParser::ParseExpr()
		{
			return ParseExpr(0);
		}

		ast::Expr* ExprParser::ParseExpr(ysize prec)
		{
			return ParseSuper(m_ExprParselets, prec);
		}

		ast::TypeDesc* ExprParser::ParseTypeDesc()
		{
			return ParseTypeDesc(0);
		}

		ast::TypeDesc* ExprParser::ParseTypeDesc(ysize prec)
		{
			return ParseSuper(m_TypeDescParselets, prec);
		}
	}
}