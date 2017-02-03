#pragma once

#include "Symbol.h"
#include "TypedSymbol.h"

namespace yk
{
	class VarSymbol : public TypedSymbol
	{
	public:
		VarSymbol(ystr const& nm, TypeSymbol* t);
	};

	class ParamSymbol : public VarSymbol
	{
	public:
		ParamSymbol(ystr const& nm, TypeSymbol* t);
	};
}