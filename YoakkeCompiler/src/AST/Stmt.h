#pragma once

#include <string>
#include "Node.h"
#include "../Parsing/Token.h"
#include "../Parsing/NodePos.h"

namespace yk
{
	class Expr;

	class Stmt : public Node
	{
	public:
		Stmt(NodePos const& p);
		virtual ~Stmt();
	};

	class ExprStmt : public Stmt
	{
	public:
		Expr* Sub;
		bool Semicol;

	public:
		ExprStmt(Expr* s, Token* sc);
		virtual ~ExprStmt();

	public:
		virtual std::string ToXML() override;
	};
}