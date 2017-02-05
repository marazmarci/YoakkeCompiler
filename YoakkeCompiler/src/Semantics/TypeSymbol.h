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

	class UnresolvedTypeSymbol : public TypeSymbol
	{
	public:
		UnresolvedTypeSymbol();

	public:
		bool Same(TypeSymbol* other) override;
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
		TypeSymbol* Parameters;
		TypeSymbol* ReturnType;

	public:
		FunctionTypeSymbol(TypeSymbol* params, TypeSymbol* rt);

	public:
		bool Same(TypeSymbol* other) override;
	};

	class TupleTypeSymbol : public TypeSymbol
	{
	public:
		TypeSymbol* LHS;
		TypeSymbol* RHS;

	public:
		TupleTypeSymbol(TypeSymbol* l, TypeSymbol* r);

	public:
		bool Same(TypeSymbol* other) override;
	};
}