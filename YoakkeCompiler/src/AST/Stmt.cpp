#include "Stmt.h"
#include "Expr.h"

namespace yk
{
	ExprStmt::ExprStmt(Expr* s)
		: Sub(s)
	{
	}

	ExprStmt::~ExprStmt()
	{
		delete Sub;
	}

	std::string ExprStmt::ToString()
	{
		return "<ExprStmt>" + Sub->ToString() + "</ExprStmt>";
	}
}