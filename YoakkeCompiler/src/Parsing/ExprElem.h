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
}