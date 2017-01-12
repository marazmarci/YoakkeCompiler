#pragma once

#include <string>
#include "AST.h"
#include "TypeDesc.h"
#include "Expr.h"

namespace yk
{

	struct Stmt : public ASTNode
	{
	public:
		virtual ~Stmt() { }
	};

	struct SingleAsgnStmt : public Stmt
	{
	public:
		std::string Label;
		TypeDesc* Type;
		Expr* Value;

	public:
		SingleAsgnStmt(std::string const& lb, TypeDesc* td, Expr* val);

	public:
		virtual std::string ToString() override;
	};
}