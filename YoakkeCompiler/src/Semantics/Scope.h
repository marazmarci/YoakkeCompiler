#pragma once

#include <map>
#include <string>
#include <vector>
#include "../Types.h"

namespace yk
{
	class Symbol;
	typedef ymap<ystr, yvec<Symbol*>> SymbolDict;

	class Scope
	{
	public:
		Scope* Parent;
		yvec<Scope*> Children;
		SymbolDict Symbols;

	public:
		Scope();

	public:
		virtual yvec<Symbol*>* Reference(ystr const& id);
		yvec<Symbol*>* ReferenceGlobal(ystr const& id);
		void Declare(Symbol* s);
	};

	class BlockScope : public Scope
	{
	public:
		BlockScope();
	};

	class FunctionScope : public Scope
	{
	public:
		FunctionScope();

	public:
		virtual yvec<Symbol*>* Reference(ystr const& id) override;
	};
}