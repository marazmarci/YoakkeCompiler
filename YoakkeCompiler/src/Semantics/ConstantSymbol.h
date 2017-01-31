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
		ConstantSymbol(std::string const& nm, Expr* val);
	};
}