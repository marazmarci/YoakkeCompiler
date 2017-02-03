#pragma once

#include "Scope.h"
#include "Symbol.h"

namespace yk
{
	class TypeSymbol;
	class TypedSymbol;
	class ConstantSymbol;

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
		yvec<Symbol*>* RefSymbol(ystr const& id);
		void DeclSymbol(Symbol* sym);

		yvec<TypedSymbol*> FilterTyped(yvec<TypedSymbol*>& syms, TypeSymbol* match);
		yvec<ConstantSymbol*> FilterArgs(yvec<ConstantSymbol*>& syms, yvec<TypeSymbol*>& args);

		inline bool GlobalScope() { return m_Current == m_Root; }

		template <typename T>
		yvec<T*> Filter(yvec<Symbol*>* syms)
		{
			yvec<T*> ret;
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
		T* FilterSingle(ystr const& id)
		{
			auto set = Filter<T>(RefSymbol(id));
			if (set.size() == 1)
				return set[0];
			return nullptr;
		}
	};
}