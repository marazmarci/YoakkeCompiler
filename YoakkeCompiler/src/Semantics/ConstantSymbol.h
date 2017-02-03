#pragma once

#include "TypedSymbol.h"
#include "Scope.h"
#include "../AST/Expr.h"

namespace yk
{
	class ConstantSymbol : public TypedSymbol
	{
	public:
		ConstantSymbol(ystr const& nm, TypeSymbol* t);
	};

	class BuiltinConstantSymbol : public ConstantSymbol
	{
	public:
		BuiltinConstantSymbol(ystr const& nm, TypeSymbol* t);
	};

	class UserConstantSymbol : public ConstantSymbol
	{
	public:
		Expr* Value;

	public:
		UserConstantSymbol(ystr const& nm, Expr* e);
	};
}