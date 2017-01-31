#pragma once

#include "ParseState.h"
#include "../AST/Expr.h"

namespace yk
{
	enum class ExprElemT
	{
		Expr, Oper
	};

	class ExprElem
	{
	public:
		ExprElemT Tag;
		Token Oper;
		Expr* Exp;

	public:
		ExprElem(Token top)
			: Tag(ExprElemT::Oper), Oper(top), Exp(nullptr)
		{
		}

		ExprElem(Expr* e)
			: Tag(ExprElemT::Expr), Exp(e), Oper(Token(TokenT::Epsilon, "", 0, 0))
		{
		}

		inline Token* GetOper()
		{
			if (Tag == ExprElemT::Oper) return &Oper;
			return nullptr;
		}

		inline Expr* GetExpr()
		{
			if (Tag == ExprElemT::Expr) return Exp;
			return nullptr;
		}
	};

	class ExprElemR
	{
	public:
		ExprElemT Tag;
		void* Value;
		Token Reference;

	public:
		ExprElemR(Expr* e)
			: Tag(ExprElemT::Expr), Value(e), Reference(Token(TokenT::Epsilon, "", 0, 0))
		{
		}

		ExprElemR(Operator* o, Token const& r)
			: Tag(ExprElemT::Oper), Value(o), Reference(r)
		{
		}

	public:
		Expr* GetExpr()
		{
			if (Tag == ExprElemT::Expr) return (Expr*)Value;
			return nullptr;
		}

		Operator* GetOper()
		{
			if (Tag == ExprElemT::Oper) return (Operator*)Value;
			return nullptr;
		}
	};
}