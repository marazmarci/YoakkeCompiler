#include "Scope.h"
#include "Symbol.h"

namespace yk
{
	Scope::Scope()
		: Parent(nullptr)
	{
	}

	yvec<Symbol*>* Scope::Reference(ystr const& id)
	{
		auto it = Symbols.find(id);
		if (it == Symbols.end())
		{
			if (Parent)
				return Parent->Reference(id);
			return nullptr;
		}
		return &it->second;
	}

	yvec<Symbol*>* Scope::ReferenceGlobal(ystr const& id)
	{
		if (Parent)
		{
			return Parent->ReferenceGlobal(id);
		}
		return Reference(id);
	}

	void Scope::Declare(Symbol* s)
	{
		auto it = Symbols.find(s->Name);
		if (it == Symbols.end())
		{
			yvec<Symbol*> set;
			set.push_back(s);
			Symbols.insert(std::make_pair(s->Name, set));
		}
		else
		{
			it->second.push_back(s);
		}
	}

	BlockScope::BlockScope()
		: Scope()
	{
	}

	FunctionScope::FunctionScope()
		: Scope()
	{
	}

	yvec<Symbol*>* FunctionScope::Reference(ystr const& id)
	{
		auto it = Symbols.find(id);
		if (it == Symbols.end())
			return ReferenceGlobal(id);
		return &it->second;
	}
}