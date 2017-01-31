#pragma once

#include "Scope.h"
#include "Symbol.h"

namespace yk
{
	class TypeSymbol;
	class TypedSymbol;

	class SymbolTable
	{
	private:
		Scope* m_Root;
		Scope* m_Current;

	public:
		SymbolTable();

	public:
		void Init();

		void PushScope(Scope* sc);
		void PopScope();
		std::vector<Symbol*>* RefSymbol(std::string const& id);
		void DeclSymbol(Symbol* sym);

		std::vector<TypedSymbol*> FilterTyped(std::vector<TypedSymbol*>& syms, TypeSymbol* match);

		inline bool GlobalScope() { return m_Current == m_Root; }

		template <typename T>
		std::vector<T*> Filter(std::vector<Symbol*>* syms)
		{
			std::vector<T*> ret;
			if (syms)
			{
				for (auto s : *syms)
				{
					if (T* ss = dynamic_cast<T*>(s))
						ret.push_back(ss);
				}
			}
			return ret;
		}

		template <typename T>
		T* FilterSingle(std::string const& id)
		{
			auto set = Filter<T>(RefSymbol(id));
			if (set.size() == 1)
				return set[0];
			return nullptr;
		}
	};
}