#pragma once

#include "SymbolTable.h"
#include "../AST/Stmt.h"
#include "../AST/Expr.h"
#include "../Logging/Logger.h"
#include "../Parsing/Token.h"

namespace yk
{
	class SemanticChecker
	{
	private:
		SymbolTable m_Table;
		Logger m_Logger;
		const char* m_Src;

	public:
		SemanticChecker(const char* src);

	public:
		void Check(std::vector<Stmt*> ls);
		void Check(Stmt* st);
		void Check(Expr* exp);
		void Check(TypeDesc* td);

	private:
		void ErrorAt(std::string const& msg, NodePos const& t);
		void WarnAt(std::string const& msg, NodePos const& t);
	};
}