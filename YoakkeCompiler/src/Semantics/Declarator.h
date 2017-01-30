#pragma once

#include "../AST/Stmt.h"
#include <vector>

namespace yk
{
	class SemanticChecker;
	class Scope;

	class Declarator
	{
	private:
		SemanticChecker& m_Checker;
		Scope* m_Target;

	public:
		Declarator(SemanticChecker& ch, Scope* tar);

	public:
		void Declare(std::vector<Stmt*>& ls);

	private:
		void Declare(Stmt* st);
		void Declare(Expr* exp);
	};
}