#pragma once

#include <map>
#include <string>
#include <vector>

namespace yk
{
	class Symbol;
	typedef std::map<std::string, std::vector<Symbol*>> SymbolDict;

	class Scope
	{
	public:
		Scope* Parent;
		std::vector<Scope*> Children;
		SymbolDict Symbols;

	public:
		Scope();

	public:
		virtual std::vector<Symbol*>* Reference(std::string const& id);
		std::vector<Symbol*>* ReferenceGlobal(std::string const& id);
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
		virtual std::vector<Symbol*>* Reference(std::string const& id) override;
	};
}