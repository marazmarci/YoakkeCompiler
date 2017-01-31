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
		SemanticChecker();

	public:
		void Check(std::vector<Stmt*> ls);
		void Check(Stmt* st);
		void Check(Expr* exp);
		void Check(TypeDesc* td);

	private:
		TypeSymbol* CheckPrototype(FuncPrototype* proto);

		void Error(std::string const& err);
	};
}