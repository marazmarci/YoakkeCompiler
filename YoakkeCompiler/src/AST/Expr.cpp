#include "Expr.h"
#include "Stmt.h"
#include "TypeDesc.h"
#include "../Parsing/Operator.h"

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

	// Integer literal
	IntLiteralExpr::IntLiteralExpr(Token const& tok)
		: Expr(NodePos::Get(tok)), Value(std::atoi(tok.Value.c_str()))
	{
	}

	IntLiteralExpr::~IntLiteralExpr()
	{
	}

	XMLNode* IntLiteralExpr::ToXML()
	{
		auto node = new XMLNode("IntLiteralExpr");
		node->Attributes.push_back(XMLAttribute("value", std::to_string(Value)));
		return node;
	}

	// Identifier expression
	IdentExpr::IdentExpr(Token const& tok)
		: Expr(NodePos::Get(tok)), Ident(tok.Value), Sym(nullptr), HintType(nullptr)
	{
	}

	IdentExpr::~IdentExpr()
	{
	}

	XMLNode* IdentExpr::ToXML()
	{
		auto node = new XMLNode("IdentExpr");
		node->Attributes.push_back(XMLAttribute("name", Ident));
		return node;
	}

	// Block expression
	BlockExpr::BlockExpr(yvec<Stmt*> const& st, NodePos const& p)
		: Expr(p), Statements(st)
	{
	}

	BlockExpr::~BlockExpr()
	{
		for (Stmt* s : Statements) delete s;
	}

	XMLNode* BlockExpr::ToXML()
	{
		auto node = new XMLNode("BlockExpr");
		for (Stmt* st : Statements) node->Children.push_back(st->ToXML());
		return node;
	}

	// Parameter
	ParamExpr::ParamExpr(Token* v, TypeDesc* t)
		: Expr(t->Position), Value(v ? v->Value : ""), Type(t)
	{
		if (v)
		{
			Position.StartX = v->Column;
			Position.StartY = v->Row;
		}
	}

	ParamExpr::~ParamExpr()
	{
		delete Type;
	}

	XMLNode* ParamExpr::ToXML()
	{
		auto node = new XMLNode("ParamExpr");
		node->Attributes.push_back(XMLAttribute("name", Value));
		node->Children.push_back(Type->ToXML());
		return node;
	}

	// Function declaration
	FuncHeaderExpr::FuncHeaderExpr(yvec<ParamExpr*> const& pars, TypeDesc* ret, NodePos const& p)
		: Expr(p), Parameters(pars), ReturnType(ret)
	{
	}

	FuncHeaderExpr::~FuncHeaderExpr()
	{
		delete ReturnType;
		for (auto p : Parameters) delete p;
	}

	XMLNode* FuncHeaderExpr::ToXML()
	{
		auto node = new XMLNode("FuncHeaderExpr");
		node->Children.push_back(ReturnType->ToXML());
		for (auto td : Parameters) node->Children.push_back(td->ToXML());
		return node;
	}

	// Function definition
	FuncExpr::FuncExpr(FuncHeaderExpr* proto, BlockExpr* body)
		: Expr(NodePos::Join(proto, body)), Prototype(proto), Body(body)
	{
	}

	FuncExpr::~FuncExpr()
	{
		delete Prototype;
		delete Body;
	}

	XMLNode* FuncExpr::ToXML()
	{
		auto node = new XMLNode("<FuncExpr>");
		node->Children.push_back(Prototype->ToXML());
		node->Children.push_back(Body->ToXML());
		return node;
	}

	// Operator
	OperExpr::OperExpr(Operator* o, Token const& t)
		: Expr(NodePos::Get(t)), OP(o)
	{
	}

	OperExpr::~OperExpr()
	{
	}

	XMLNode* OperExpr::ToXML()
	{
		auto node = new XMLNode("OperExpr");
		node->Attributes.push_back(XMLAttribute("name", OP->Symbol));
		return node;
	}

	// Unary expression
	UryExpr::UryExpr(Expr* s, OperExpr* o)
		: Expr(NodePos::Join(s, o)), Sub(s), OP(o)
	{
	}

	UryExpr::~UryExpr()
	{
		delete Sub;
		delete OP;
	}

	XMLNode* UryExpr::ToXML()
	{
		auto node = new XMLNode("UryExpr");
		node->Children.push_back(OP->ToXML());
		node->Children.push_back(Sub->ToXML());
		return node;
	}

	// Binary expression
	BinExpr::BinExpr(Expr* l, Expr* r, OperExpr* o)
		: Expr(l->Position), LHS(l), RHS(r), OP(o)
	{
		Position.EndX = r->Position.EndX;
		Position.EndY = r->Position.EndY;
	}

	BinExpr::~BinExpr()
	{
		delete LHS;
		delete RHS;
	}

	XMLNode* BinExpr::ToXML()
	{
		auto node = new XMLNode("BinExpr");
		node->Children.push_back(LHS->ToXML());
		node->Children.push_back(OP->ToXML());
		node->Children.push_back(RHS->ToXML());
		return node;
	}

	// Enclosed expression
	EnclosedExpr::EnclosedExpr(Expr* s, Token const& l, Token const& r)
		: Expr(NodePos::Interval(l, r)), Begin(l), End(r)
	{
	}

	EnclosedExpr::~EnclosedExpr()
	{
		delete Sub;
	}

	XMLNode* EnclosedExpr::ToXML()
	{
		auto node = new XMLNode("EnclosedExpr");
		node->Attributes.push_back(XMLAttribute("begin", Begin.Value));
		node->Attributes.push_back(XMLAttribute("end", End.Value));
		node->Children.push_back(Sub->ToXML());
		return node;
	}
}