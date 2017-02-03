#pragma once

#include "TypedSymbol.h"
#include "Scope.h"
#include "../AST/Expr.h"

namespace yk
{
	class ConstantSymbol : public TypedSymbol
	{
	public:
		Expr* Value;

	public:
		ConstantSymbol(ystr const& nm, Expr* val);
	};
}