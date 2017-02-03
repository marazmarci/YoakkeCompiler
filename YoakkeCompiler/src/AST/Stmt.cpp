#include "Stmt.h"
#include "Expr.h"
#include "../Parsing/NodePos.h"

namespace yk
{
	Stmt::Stmt(NodePos const& p)
		: Node(p)
	{
	}

	Stmt::~Stmt()
	{
	}

	ExprStmt::ExprStmt(Expr* s, Token* sc)
		: Stmt(s->Position), Sub(s), Semicol(sc != nullptr)
	{
		if (sc)
		{
			Position.EndX = sc->Column;
			Position.EndY = sc->Row;
		}
	}

	ExprStmt::~ExprStmt()
	{
		delete Sub;
	}

	XMLNode* ExprStmt::ToXML()
	{
		auto node = new XMLNode("ExprStmt");
		node->Children.push_back(Sub->ToXML());
		return node;
	}
}