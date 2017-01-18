#pragma once

#include <string>
#include <vector>
#include "FuncPrototype.h"

namespace yk
{
	class Stmt;
	class TypeDesc;

	class Expr
	{
	public:
		virtual ~Expr() { }

	public:
		virtual std::string ToString() = 0;
	};

	class BlockExpr : public Expr
	{
	public:
		std::vector<Stmt*> Statements;

	public:
		BlockExpr(std::vector<Stmt*> const& st);
		virtual ~BlockExpr();

	public:
		virtual std::string ToString() override;
	};

	class FuncHeaderExpr : public Expr
	{
	public:
		FuncPrototype* Prototype;

	public:
		FuncHeaderExpr(FuncPrototype* proto);
		virtual ~FuncHeaderExpr();

	public:
		virtual std::string ToString() override;
	};

	class FuncExpr : public Expr
	{
	public:
		FuncPrototype* Prototype;
		BlockExpr* Body;

	public:
		FuncExpr(FuncPrototype* proto, BlockExpr* body);
		virtual ~FuncExpr();

	public:
		virtual std::string ToString() override;
	};
}