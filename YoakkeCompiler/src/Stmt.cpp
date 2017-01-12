#include "Stmt.h"

namespace yk
{
	SingleAsgnStmt::SingleAsgnStmt(std::string const& lb, TypeDesc* td, Expr* val)
		: Label(lb), Type(td), Value(val)
	{
	}

	std::string SingleAsgnStmt::ToString()
	{
		return "<SingleAssignment label=\"" + Label + "\">" +
			(Type ? Type->ToString() : "") +
			Value->ToString() +
			"</BinaryExpr>";
	}
}