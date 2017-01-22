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

	class ExprStmt : public Stmt
	{
	public:
		Expr* Sub;

	public:
		ExprStmt(Expr* s);
		virtual ~ExprStmt();

	public:
		virtual std::string ToString() override;
	};
}