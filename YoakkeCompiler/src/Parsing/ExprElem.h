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
		ExprElem(Token top);
		ExprElem(Expr* e);

	public:
		Token* GetOper();
		Expr* GetExpr();
	};

	class ExprElemR
	{
	public:
		ExprElemT Tag;
		void* Value;
		Token Reference;

	public:
		ExprElemR(Expr* e);
		ExprElemR(Operator* o, Token const& r);

	public:
		Expr* GetExpr();
		Operator* GetOper();
	};
}