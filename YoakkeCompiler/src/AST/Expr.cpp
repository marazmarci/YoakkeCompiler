#include "Expr.h"
#include "Stmt.h"

namespace yk
{
	Expr::Expr()
		: EvalType(nullptr)
	{
	}

	IdentExpr::IdentExpr(std::string const& id, Token const& ref)
		: Ident(id), Reference(ref), Sym(nullptr), HintType(nullptr)
	{
	}

	std::string IdentExpr::ToString()
	{
		return "<IdentExpr id=\"" + Ident + "\" />";
	}

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

	UryExpr::UryExpr(Expr* s, UryOp* o)
		: Sub(s), OP(o)
	{
	}

	UryExpr::~UryExpr()
	{
		delete Sub;
	}

	std::string UryExpr::ToString()
	{
		return "<UryExpr op=\"" + OP->Symbol + "\">" + Sub->ToString() + "</UryExpr>";
	}

	BinExpr::BinExpr(Expr* l, Expr* r, BinOp* o, Token const& ref)
		: LHS(l), RHS(r), OP(o), Reference(ref)
	{
	}

	BinExpr::~BinExpr()
	{
		delete LHS;
		delete RHS;
	}

	std::string BinExpr::ToString()
	{
		return "<BinExpr op=\"" + OP->Symbol + "\">" + LHS->ToString() + RHS->ToString() + "</BinExpr>";
	}
}