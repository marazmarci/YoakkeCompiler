#include "ExprParser.h"
#include "Parselets/PrefixParselet.h"
#include "Parselets/InfixParselet.h"
#include "../Utility/MapExtension.h"

namespace yk
{
	namespace parse
	{
		ExprParser::ExprParser(Lexer* lexer, yvec<Token>* tokbuf, dbg::Logger* logger, ystr const& fn)
			: Parser(lexer, tokbuf, logger, fn)
		{
		}

		template <ysize IDX, typename PT, typename PAT>
		static void register_str(Lexer* lexer, PT& pack, ystr const& sym, PAT const& parselet)
		{
			lexer->RegisterSymbol(sym);
			auto& map = std::get<IDX>(pack);
			map.insert(std::make_pair(sym, parselet));
		}

		template <ysize IDX, typename PT, typename PAT>
		static void register_tok(Lexer* lexer, PT& pack, TokenT sym, PAT const& parselet)
		{
			auto& map = std::get<IDX>(pack);
			map.insert(std::make_pair(sym, parselet));
		}

		void ExprParser::Register(ystr const& sym, PrefixExprParselet* parselet)
		{
			register_str<1>(m_Lexer, m_ExprParselets, sym, parselet);
		}

		void ExprParser::Register(ystr const& sym, InfixExprParselet* parselet)
		{
			register_str<3>(m_Lexer, m_ExprParselets, sym, parselet);
		}

		void ExprParser::Register(TokenT tt, PrefixExprParselet* parselet)
		{
			register_tok<0>(m_Lexer, m_ExprParselets, tt, parselet);
		}

		void ExprParser::Register(TokenT tt, InfixExprParselet* parselet)
		{
			register_tok<2>(m_Lexer, m_ExprParselets, tt, parselet);
		}

		void ExprParser::Register(ystr const& sym, PrefixTypeDescParselet* parselet)
		{
			register_str<1>(m_Lexer, m_TypeDescParselets, sym, parselet);
		}

		void ExprParser::Register(ystr const& sym, InfixTypeDescParselet* parselet)
		{
			register_str<3>(m_Lexer, m_TypeDescParselets, sym, parselet);
		}

		void ExprParser::Register(TokenT tt, PrefixTypeDescParselet* parselet)
		{
			register_tok<0>(m_Lexer, m_TypeDescParselets, tt, parselet);
		}

		void ExprParser::Register(TokenT tt, InfixTypeDescParselet* parselet)
		{
			register_tok<2>(m_Lexer, m_TypeDescParselets, tt, parselet);
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