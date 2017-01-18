#pragma once

#include <string>

namespace yk
{
	class Expr;

	class Stmt
	{
	public:
		virtual ~Stmt() { }

	public:
		virtual std::string ToString() = 0;
	};

	class ConstDeclStmt : public Stmt
	{
	public:
		std::string Identifier;
		Expr* Value;

	public:
		ConstDeclStmt(std::string const& id, Expr* v);
		virtual ~ConstDeclStmt();

	public:
		virtual std::string ToString() override;
	};
}