#include "Stmt.h"
#include "Expr.h"

namespace yk
{
	ExprStmt::ExprStmt(Expr* s, bool sc)
		: Sub(s), Semicol(sc)
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