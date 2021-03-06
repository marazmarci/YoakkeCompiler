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
		protected:
			Lexer* m_Lexer;
			yvec<Token>* m_TokenBuffer;
			dbg::Logger* m_Logger;
			ystr m_File;

		protected:
			Parser(Lexer* lexer, yvec<Token>* tokbuf, dbg::Logger* logger, ystr const& fn);

		public:
			bool Match(ystr const& expect);
			bool Expect(ystr const& expect);
			Token Consume();
			Token Peek(ysize delta);
		};
	}
}