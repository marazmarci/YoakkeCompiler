#include "Expr.h"
#include "Stmt.h"
#include "TypeDesc.h"

namespace yk
{
	// Expression
	Expr::Expr(NodePos const& p)
		: Node(p), EvalType(nullptr)
	{
	}

	Expr::~Expr()
	{
	}

	// Identifier expression
	IdentExpr::IdentExpr(Token const& tok)
		: Expr(NodePos(tok.Column, tok.Row, tok.Column + tok.Value.size(), tok.Row)),
		Ident(tok.Value), Sym(nullptr), HintType(nullptr)
	{
	}

	IdentExpr::~IdentExpr()
	{
	}

	std::string IdentExpr::ToXML()
	{
		return "<IdentExpr id=\"" + Ident + "\" />";
	}

	// Block expression
	BlockExpr::BlockExpr(std::vector<Stmt*> const& st, NodePos const& p)
		: Expr(p), Statements(st)
	{
	}

	BlockExpr::~BlockExpr()
	{
		for (Stmt* s : Statements) delete s;
	}

	std::string BlockExpr::ToXML()
	{
		std::string ret = "<Block>";
		for (Stmt* st : Statements) ret += st->ToXML();
		ret += "</Block>";
		return ret;
	}

	// Parameter
	Parameter::Parameter(Token const& v, TypeDesc* t)
		: Value(v), Type(t)
	{
	}

	Parameter::~Parameter()
	{
	}

	// Function declaration
	FuncHeaderExpr::FuncHeaderExpr(std::vector<Parameter> const& pars, TypeDesc* ret, NodePos const& p)
		: Expr(p), Parameters(pars), ReturnType(ret)
	{
	}

	FuncHeaderExpr::~FuncHeaderExpr()
	{
		delete ReturnType;
	}

	std::string FuncHeaderExpr::ToXML()
	{
		std::string ret = "<FuncHeader><ReturnType>" +
			ReturnType->ToXML() +
			"</ReturnType>";
		for (auto td : Parameters)
		{
			ret += "<Parameter name=\"" + td.Value.Value + "\">" +
				td.Type->ToXML() +
				"</Parameter>";
		}
		ret += "</FuncHeader>";
		return ret;
	}

	// Function definition
	FuncExpr::FuncExpr(FuncHeaderExpr* proto, BlockExpr* body, NodePos const& p)
		: Expr(p), Prototype(proto), Body(body)
	{
	}

	FuncExpr::~FuncExpr()
	{
		delete Prototype;
		delete Body;
	}

	std::string FuncExpr::ToXML()
	{
		return "<FuncExpr>" + 
			Prototype->ToXML() + Body->ToXML() +
			"</FuncExpr>";
	}

	// Unary expression
	UryExpr::UryExpr(Expr* s, OperPos const& o)
		: Expr(s->Position), Sub(s), OP(o)
	{
		if (((UryOp*)o.OP)->Fixity == FixityT::Prefix)
		{
			Position.StartX = o.StartX;
			Position.StartY = o.StartY;
		}
		else
		{
			Position.EndX = o.EndX;
			Position.EndY = o.EndY;
		}
	}

	UryExpr::~UryExpr()
	{
		delete Sub;
	}

	std::string UryExpr::ToXML()
	{
		return "<UryExpr op=\"" + OP.OP->Symbol + "\">" + 
			Sub->ToXML() + 
			"</UryExpr>";
	}

	// Binary expression
	BinExpr::BinExpr(Expr* l, Expr* r, OperPos const& o)
		: Expr(NodePos(l->Position.StartX, l->Position.StartY, 
			r->Position.EndX, r->Position.EndY)), 
		LHS(l), RHS(r), OP(o)
	{
	}

	BinExpr::~BinExpr()
	{
		delete LHS;
		delete RHS;
	}

	std::string BinExpr::ToXML()
	{
		return "<BinExpr op=\"" + OP.OP->Symbol + "\">" + 
			LHS->ToXML() + RHS->ToXML() + 
			"</BinExpr>";
	}

	// Enclosed expression
	EnclosedExpr::EnclosedExpr(Expr* s, Token const& l, Token const& r)
		: Expr(NodePos(l.Column, l.Row, r.Column, r.Row)), Begin(l), End(r)
	{
	}

	EnclosedExpr::~EnclosedExpr()
	{
		delete Sub;
	}

	std::string EnclosedExpr::ToXML()
	{
		return "<EnclosedExpr begin=\"" + Begin.Value + "end = \"" + End.Value + "\">" +
			Sub->ToXML() +
			"</EnclosedExpr>";
	}
}