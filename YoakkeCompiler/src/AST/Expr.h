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

	template <typename T>
	class LiteralExpr : public Expr
	{
	public:
		T Value;

	public:
		LiteralExpr(T const& v)
			: Value(v)
		{
		}

	public:
		virtual std::string ToString() override
		{
			std::string name = std::string(typeid(T).name()) + "Literal";
			return "<" + name + ">" + std::to_string(Value) + "</" + name + ">";
		}
	};
	typedef LiteralExpr<int> IntLiteralExpr;

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

	class BinExpr : public Expr
	{
	public:
		Expr* LHS;
		Expr* RHS;
		std::string OP;

	public:
		BinExpr(Expr* l, Expr* r, std::string o);
		virtual ~BinExpr();

	public:
		virtual std::string ToString() override;
	};
}