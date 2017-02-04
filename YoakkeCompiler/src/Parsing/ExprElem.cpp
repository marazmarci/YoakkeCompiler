#include "ExprElem.h"
#include "Operator.h"

namespace yk
{
	// Expression element
	ExprElem::ExprElem(Token top)
		: Tag(ExprElemT::Oper), Oper(top), Exp(nullptr)
	{
	}

	ExprElem::ExprElem(Expr* e)
		: Tag(ExprElemT::Expr), Exp(e), Oper(Token(TokenT::Epsilon, "", 0, 0))
	{
	}

	Token* ExprElem::GetOper()
	{
		if (Tag == ExprElemT::Oper) return &Oper;
		return nullptr;
	}

	Expr* ExprElem::GetExpr()
	{
		if (Tag == ExprElemT::Expr) return Exp;
		return nullptr;
	}

	// Reduced expression element
	ExprElemR::ExprElemR(Expr* e)
		: Tag(ExprElemT::Expr), ExpValue(e), OperValue(nullptr), 
		Reference(Token(TokenT::Epsilon, "", 0, 0))
	{
	}

	ExprElemR::ExprElemR(Operator* o, Token const& r)
		: Tag(ExprElemT::Oper), OperValue(o), ExpValue(nullptr), Reference(r)
	{
	}

	Expr* ExprElemR::GetExpr()
	{
		if (Tag == ExprElemT::Expr) return ExpValue;
		return nullptr;
	}

	Operator* ExprElemR::GetOper()
	{
		if (Tag == ExprElemT::Oper) return OperValue;
		return nullptr;
	}
}