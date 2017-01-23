#include <iostream>
#include "SemanticChecker.h"

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

	}
}