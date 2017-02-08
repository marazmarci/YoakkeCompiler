#pragma once

#include "LexerState.h"
#include "Token.h"
#include "../Utility/Iterator.h"
#include "../Common.h"
#include "../Debug/Logger.h"

namespace yk
{
	namespace parse
	{
		class Lexer : public Iterator<Token, LexerState>
		{
		private:
			LexerState m_State;
			yset<ystr> m_Symbols;
			yset<ystr> m_Keywords;
			ystr m_File;
			dbg::Logger m_Logger;

		public:
			Lexer();

		public:
			void SetSource(const char* buf, ystr const& fn);

			// Register words
			void RegisterSymbol(ystr const& kw);
			void RegisterKeyword(ystr const& kw);

			// Override Iterator<T, U> methods
			Token		Next()		override;
			bool		HasNext()	override;
			LexerState	Position()	override;
			void		Rollback(LexerState const& pos) override;
		};
	}
}