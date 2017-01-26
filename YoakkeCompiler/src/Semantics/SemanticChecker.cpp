#include <iostream>
#include "SemanticChecker.h"
#include "../AST/Expr.h"

namespace yk
{
	void SemanticChecker::Error(std::string const& msg)
	{
		std::cout << "Semantic error: " << msg << std::endl;
	}

	void SemanticChecker::Check(Stmt* st)
	{
		if (ExprStmt* es = dynamic_cast<ExprStmt*>(st))
		{
			Check(es->Sub);
		}
	}

	void SemanticChecker::Check(Expr* exp)
	{
		if (BinExpr* be = dynamic_cast<BinExpr*>(exp))
		{
			if (be->OP->Symbol == "::")
			{
				// Constant binding
				if (IdentExpr* ie = dynamic_cast<IdentExpr*>(be->LHS))
				{
					
				}
				else
				{
					Error("LHS of constant binding must be an identifier!");
				}
			}
		}
	}

	bool SemanticChecker::Constant(Expr* exp)
	{
		if (dynamic_cast<FuncExpr*>(exp))
		{
			return true;
		}

		return false;
	}
}