#pragma once

#include "SymbolTable.h"
#include "../AST/Stmt.h"
#include "../AST/Expr.h"

namespace yk
{
	class SemanticChecker
	{
	private:
		SymbolTable m_Table;

	public:
		bool Check(Stmt* st);
		bool Check(Expr* exp);

	private:
		FunctionSymbol* Symbolize(std::string const& id, FuncPrototype* fe);
		TypeSymbol* Symbolize(TypeDesc* td);

		void Error(std::string const& err);
	};
}