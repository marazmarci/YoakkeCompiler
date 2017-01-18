#include "Stmt.h"
#include "Expr.h"

namespace yk
{
	ConstDeclStmt::ConstDeclStmt(std::string const& id, Expr* v)
		: Identifier(id), Value(v)
	{
	}

	ConstDeclStmt::~ConstDeclStmt()
	{
		delete Value;
	}

	std::string ConstDeclStmt::ToString()
	{
		return "<ConstDeclStmt id=\"" + Identifier + "\">" + Value->ToString() + "</ConstDeclStmt>";
	}
}