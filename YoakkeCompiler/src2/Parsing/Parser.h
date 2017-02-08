#pragma once

#include "YExprParser.h"
#include "Parser.h"
#include "../Common.h"
#include "../Debug/Logger.h"

namespace yk
{
	namespace parse
	{
		class Stmt;

		class Parser
		{
		private:
			dbg::Logger m_Logger;
			Lexer m_Lexer;
			ystr const& m_File;

		public:
			Parser(const char* buf, ystr const& fn);

		public:
			yvec<Stmt*> ParseProgram();
		};
	}
}