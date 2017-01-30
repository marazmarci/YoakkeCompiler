#pragma once

#include "Scope.h"

namespace yk
{
	class SymbolTable
	{
	private:
		Scope* m_Root;
		Scope* m_Current;

	public:
		SymbolTable();

	public:
		void PushScope(Scope* sc);
		void PopScope();
		std::vector<Symbol*>* RefSymbol(std::string const& id);
		void DeclSymbol(Symbol* sym);
		FunctionSymbol* RefFunction(FunctionSymbol* f);

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
	};
}