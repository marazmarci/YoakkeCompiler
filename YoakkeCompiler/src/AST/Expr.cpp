#include "Expr.h"
#include "Stmt.h"

namespace yk
{
	BlockExpr::BlockExpr(std::vector<Stmt*> const& st)
		: Statements(st)
	{
	}

	BlockExpr::~BlockExpr()
	{
		for (Stmt* s : Statements) delete s;
	}

	std::string BlockExpr::ToString()
	{
		std::string ret = "<Block>";
		for (Stmt* st : Statements) ret += st->ToString();
		ret += "</Block>";
		return ret;
	}

	FuncHeaderExpr::FuncHeaderExpr(FuncPrototype* proto)
		: Prototype(proto)
	{
	}

	FuncHeaderExpr::~FuncHeaderExpr()
	{
		delete Prototype;
	}

	std::string FuncHeaderExpr::ToString()
	{
		return "<FuncHeaderExpr>" + Prototype->ToString() + "</FuncHeaderExpr>";
	}

	FuncExpr::FuncExpr(FuncPrototype* proto, BlockExpr* body)
		: Prototype(proto), Body(body)
	{
	}

	FuncExpr::~FuncExpr()
	{
		delete Prototype;
		delete Body;
	}

	std::string FuncExpr::ToString()
	{
		return "<FuncExpr>" + Prototype->ToString() + Body->ToString() + "</FuncExpr>";
	}
}