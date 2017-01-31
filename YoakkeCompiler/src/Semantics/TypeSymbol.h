#pragma once

#include <vector>
#include "Symbol.h"

namespace yk
{
	class TypeSymbol : public Symbol
	{
	public:
		TypeSymbol(std::string const& n);

	public:
		virtual bool Same(TypeSymbol* other) = 0;
	};

	class ResolvableTypeSymbol : public TypeSymbol
	{
	public:
		TypeSymbol* Resolved;

	public:
		ResolvableTypeSymbol(std::string const& n);
	};

	class BuiltinTypeSymbol : public TypeSymbol
	{
	public:
		BuiltinTypeSymbol(std::string const& n);

	public:
		bool Same(TypeSymbol* other) override;
	};

	class FunctionTypeSymbol : public TypeSymbol
	{
	public:
		std::vector<TypeSymbol*> Parameters;
		TypeSymbol* ReturnType;

	public:
		FunctionTypeSymbol(std::vector<TypeSymbol*>& params, TypeSymbol* rt);

	public:
		bool Same(TypeSymbol* other) override;
	};
}