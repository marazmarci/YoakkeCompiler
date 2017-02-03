#pragma once

#include <vector>
#include "Symbol.h"

namespace yk
{
	class TypeSymbol : public Symbol
	{
	public:
		TypeSymbol(ystr const& n);

	public:
		virtual bool Same(TypeSymbol* other) = 0;
	};

	class ResolvableTypeSymbol : public TypeSymbol
	{
	public:
		TypeSymbol* Resolved;

	public:
		ResolvableTypeSymbol(ystr const& n);
	};

	class BuiltinTypeSymbol : public TypeSymbol
	{
	public:
		BuiltinTypeSymbol(ystr const& n);

	public:
		bool Same(TypeSymbol* other) override;
	};

	class FunctionTypeSymbol : public TypeSymbol
	{
	public:
		yvec<TypeSymbol*> Parameters;
		TypeSymbol* ReturnType;

	public:
		FunctionTypeSymbol(yvec<TypeSymbol*>& params, TypeSymbol* rt);

	public:
		bool Same(TypeSymbol* other) override;
	};

	class TupleTypeSymbol : public TypeSymbol
	{
	public:
		yvec<TypeSymbol*> Types;

	public:
		TupleTypeSymbol(yvec<TypeSymbol*> const& ls);

	public:
		bool Same(TypeSymbol* other) override;
	};
}