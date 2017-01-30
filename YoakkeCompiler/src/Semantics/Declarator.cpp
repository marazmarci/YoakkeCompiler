#include <iostream>
#include "Declarator.h"
#include "ConstantEvaluator.h"
#include "SemanticChecker.h"
#include "Scope.h"
#include "../AST/Expr.h"
#include "FunctionSymbol.h"

namespace yk
{
	Declarator::Declarator(SemanticChecker& ch, Scope* tar)
		: m_Checker(ch), m_Target(tar)
	{
	}

	void Declarator::Declare(std::vector<Stmt*>& ls)
	{
		for (auto el : ls)
			Declare(el);
	}

	void Declarator::Declare(Stmt* st)
	{
		if (ExprStmt* es = dynamic_cast<ExprStmt*>(st))
		{
			Declare(es);
		}
	}

	void Declarator::Declare(Expr* exp)
	{
		if (BinExpr* be = dynamic_cast<BinExpr*>(exp))
		{
			if (be->OP->Symbol == "::")
			{
				m_Checker.Check(be);
			}
		}
	}
}