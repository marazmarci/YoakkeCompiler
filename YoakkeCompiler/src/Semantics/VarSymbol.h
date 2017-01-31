#pragma once

#include "Symbol.h"
#include "TypedSymbol.h"

namespace yk
{
	class VarSymbol : public TypedSymbol
	{
	public:
		VarSymbol(std::string const& nm, TypeSymbol* t);
	};

	class ParamSymbol : public VarSymbol
	{
	public:
		ParamSymbol(std::string const& nm, TypeSymbol* t);
	};
}