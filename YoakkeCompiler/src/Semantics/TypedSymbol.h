#pragma once

#include "Symbol.h"
#include "TypeSymbol.h"

namespace yk
{
	class TypedSymbol : public Symbol
	{
	public:
		TypeSymbol* Type;

	public:
		TypedSymbol(ystr const& nm, TypeSymbol* t);
	};
}