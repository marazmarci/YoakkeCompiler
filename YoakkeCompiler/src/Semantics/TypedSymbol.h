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
		TypedSymbol(std::string const& nm, TypeSymbol* t);
	};
}