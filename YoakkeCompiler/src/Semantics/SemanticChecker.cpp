#include <iostream>
#include "SemanticChecker.h"
#include "ConstantEvaluator.h"
#include "FunctionSymbol.h"
#include "../AST/FuncPrototype.h"

namespace yk
{
	bool SemanticChecker::Check(Stmt* st)
	{
		
	}

	bool SemanticChecker::Check(Expr* exp)
	{
		if (BinExpr* be = dynamic_cast<BinExpr*>(exp))
		{
			auto LHS = be->LHS;
			auto RHS = be->RHS;

			if (be->OP->Symbol == "::")
			{
				// This could be called from a Declarator
				// TODO: second pass

				if (IdentExpr* ie = dynamic_cast<IdentExpr*>(LHS))
				{
					if (Expr* rval = ConstantEvaluator::Evaluate(RHS))
					{
						
					}
					else
					{
						Error("RHS of constant binding must be a constant value!");
					}
				}
				else
				{
					Error("LHS of constant binding must be an identifier");
				}
			}
		}
	}

	FunctionSymbol* SemanticChecker::Symbolize(std::string const& id, FuncPrototype* fe)
	{
		TypeSymbol* rett = Symbolize(fe->ReturnType);

		return nullptr;
	}

	TypeSymbol* SemanticChecker::Symbolize(TypeDesc* td)
	{

	}

	void SemanticChecker::Error(std::string const& err)
	{
		std::cout << "Semantic error: " << err << std::endl;
	}
}