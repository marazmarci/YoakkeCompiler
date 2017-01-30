#include "Scope.h"
#include "Symbol.h"

namespace yk
{
	Scope::Scope()
		: Parent(nullptr)
	{
	}

	std::vector<Symbol*>* Scope::Reference(std::string const& id)
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

	void Scope::Declare(Symbol* s)
	{
		auto it = Symbols.find(s->Name);
		if (it == Symbols.end())
		{
			std::vector<Symbol*> set;
			set.push_back(s);
			Symbols.insert(std::make_pair(s->Name, set));
		}
		else
		{
			it->second.push_back(s);
		}
	}
}