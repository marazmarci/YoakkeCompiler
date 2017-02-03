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
		: Tag(ExprElemT::Expr), Value(e), Reference(Token(TokenT::Epsilon, "", 0, 0))
	{
	}

	ExprElemR::ExprElemR(Operator* o, Token const& r)
		: Tag(ExprElemT::Oper), Value(o), Reference(r)
	{
	}

	Expr* ExprElemR::GetExpr()
	{
		if (Tag == ExprElemT::Expr) return (Expr*)Value;
		return nullptr;
	}

	Operator* ExprElemR::GetOper()
	{
		if (Tag == ExprElemT::Oper) return (Operator*)Value;
		return nullptr;
	}
}