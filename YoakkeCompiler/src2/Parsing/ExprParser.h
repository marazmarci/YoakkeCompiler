#pragma once

#include "Parser.h"
#include "../Common.h"
#include "../AST/Expr.h"
#include "../AST/TypeDesc.h"
#include "../Debug/Logger.h"
#include "../Parsing/Parselets/ExprParselets.h"

namespace yk
{
	namespace parse
	{
		class YStmtParser;
		class Expr;

		class ExprParser : public Parser
		{
		private:
			template <typename T, typename U>
			using ParseletPack = yquad<
				ymap<TokenT,	T*>,
				ymap<ystr,		T*>,
				ymap<TokenT,	U*>,
				ymap<ystr,		U*>>;

		private:
			ParseletPack<PrefixExprParselet, InfixExprParselet> m_ExprParselets;
			ParseletPack<PrefixTypeDescParselet, InfixTypeDescParselet> m_TypeDescParselets;

		protected:
			ExprParser(Lexer* lexer, yvec<Token>* tokbuf, dbg::Logger* logger, ystr const& fn);

		public:
			void Register(ystr const& sym, PrefixExprParselet* parselet);
			void Register(ystr const& sym, InfixExprParselet* parselet);
			void Register(TokenT tt, PrefixExprParselet* parselet);
			void Register(TokenT tt, InfixExprParselet* parselet);

			void Register(ystr const& sym, PrefixTypeDescParselet* parselet);
			void Register(ystr const& sym, InfixTypeDescParselet* parselet);
			void Register(TokenT tt, PrefixTypeDescParselet* parselet);
			void Register(TokenT tt, InfixTypeDescParselet* parselet);

			ast::Expr* ParseExpr();
			ast::Expr* ParseExpr(ysize prec);
			ast::TypeDesc* ParseTypeDesc();
			ast::TypeDesc* ParseTypeDesc(ysize prec);

		private:
			template <typename T, typename U>
			ysize GetPrecedence(ParseletPack<T, U> const& pack)
			{
				auto& map = std::get<3>(pack);
				auto parser = map.find(Peek(0).Value);
				if (parser == map.end())
					return 0;
				return parser->second->Precedence;
			}

			template <typename T, typename U>
			auto ParseSuper(ParseletPack<T, U> const& pack, ysize prec)
			{
				auto& pref_parselets = std::get<1>(pack);
				auto& pref_parselets_T = std::get<0>(pack);
				auto& inf_parselets = std::get<3>(pack);
				auto& inf_parselets_T = std::get<2>(pack);

				auto save = m_Lexer->Position();
				auto lookahead = Consume();
				T* pre_parselet = nullptr;
				if (!(pre_parselet = ext::GetValue(pref_parselets, lookahead.Value))
					&& !(pre_parselet = ext::GetValue(pref_parselets_T, lookahead.Type)))
				{
					m_Lexer->Rollback(save);
					return (T::ReturnType)nullptr;
				}
				auto left = pre_parselet->Parse(this, lookahead);
				if (left)
				{
					while (prec < GetPrecedence(pack))
					{
						save = m_Lexer->Position();
						lookahead = Consume();
						U* in_parselet = nullptr;
						if (!(in_parselet = ext::GetValue(inf_parselets, lookahead.Value))
							&& !(in_parselet = ext::GetValue(inf_parselets_T, lookahead.Type)))
						{
							std::cout << "ERROR2" << std::endl;
							return (T::ReturnType)nullptr;
						}
						auto newleft = in_parselet->Parse(this, left, lookahead);
						if (newleft)
						{
							left = newleft;
						}
						else
						{
							m_Lexer->Rollback(save);
						}
					}
				}
				else
				{
					m_Lexer->Rollback(save);
				}
				return left;
			}
		};
	}
}