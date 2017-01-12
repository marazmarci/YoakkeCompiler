#include "Expr.h"

namespace yk
{
	// Binary expression
	BinExpr::BinExpr(Expr* lhs, Expr* rhs, Operator* op)
		: LHS(lhs), RHS(rhs), OP(op)
	{
	}

	BinExpr::~BinExpr()
	{
		delete LHS;
		delete RHS;
	}

	std::string BinExpr::ToString()
	{
		return "<BinaryExpr op=\"" + OP->Symbol + "\">" +
			LHS->ToString() + RHS->ToString() +
			"</BinaryExpr>";
	}

	// Variable expression
	VarExpr::VarExpr(std::string const& id)
		: Identifier(id)
	{
	}

	std::string VarExpr::ToString()
	{
		return "<VarExpr>" + Identifier + "</VarExpr>";
	}
}