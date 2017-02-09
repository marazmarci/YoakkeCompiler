#pragma once

#include "Lexer.h"
#include "Token.h"
#include "../Common.h"
#include "../Debug/Logger.h"

namespace yk
{
	namespace parse
	{
		class Parser
		{
		private:
			Lexer& m_Lexer;
			dbg::Logger& m_Logger;
			yvec<Token> m_TokenBuffer;
		
		protected:
			ystr m_File;

		protected:
			Parser(Lexer& lexer, dbg::Logger& logger, ystr const& fn);

		protected:
			bool Match(ystr const& expect);
			bool Expect(ystr const& expect);
			Token Consume();
			Token Peek(ysize delta);
		};
	}
}