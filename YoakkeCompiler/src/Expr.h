#pragma once

#include <string>
#include <sstream>
#include "AST.h"
#include "Operator.h"

namespace yk
{
	struct Expr : public ASTNode
	{
	public:
		virtual ~Expr() { }
	};

	struct BinExpr : public Expr
	{
	public:
		Expr* LHS;
		Expr* RHS;
		Operator* OP;

	public:
		BinExpr(Expr* lhs, Expr* rhs, Operator* op)
			: LHS(lhs), RHS(rhs), OP(op)
		{
		}

		~BinExpr()
		{
			delete LHS;
			delete RHS;
		}

	public:
		virtual std::string ToString() override
		{
			return "<BinaryExpr op=\"" + OP->Symbol + "\">" + 
				LHS->ToString() + RHS->ToString() +
				"</BinaryExpr>";
		}
	};

	template <typename T>
	struct ConstExpr : public Expr
	{
	public:
		T Value;

	public:
		ConstExpr(T val)
			: Value(val)
		{
		}

		virtual std::string ToString() override
		{
			std::stringstream ss;
			ss << Value;
			return "<Constant>" + ss.str() + "</Constant>";
		}
	};
	typedef ConstExpr<int> IntConstExpr;
	typedef ConstExpr<float> FloatConstExpr;

	struct VarExpr : public Expr
	{
	public:
		std::string Identifier;

	public:
		VarExpr(std::string const& id)
			: Identifier(id)
		{
		}

		virtual std::string ToString() override
		{
			return "<VarExpr>" + Identifier + "</VarExpr>";
		}
	};
}