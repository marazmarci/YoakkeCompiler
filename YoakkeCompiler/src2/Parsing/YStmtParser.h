#pragma once

#include "Parser.h"
#include "../AST/Stmt.h"

namespace yk
{
	namespace parse
	{
		class YExprParser;

		class YStmtParser : public Parser
		{
		private:
			YExprParser* m_ExprParser;

		public:
			YStmtParser(const char* buf, ystr const& fn);

		public:
			ast::Stmt* ParseStmt();
		};
	}
}