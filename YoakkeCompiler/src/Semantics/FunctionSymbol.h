#pragma once

#include "Symbol.h"
#include "Scope.h"

namespace yk
{
	class TypeSymbol;

	class ParamSymbol : public Symbol
	{
	public:
		TypeSymbol* Type;

	public:
		ParamSymbol(std::string const& n, TypeSymbol* t);
	};


	class FunctionSymbol : public Scope, public Symbol
	{
	public:
		std::vector<ParamSymbol*> Parameters;
		TypeSymbol* ReturnType;

	public:
		FunctionSymbol(std::string const& n, TypeSymbol* ret);

	public:
		bool Same(FunctionSymbol* other);
	};
}