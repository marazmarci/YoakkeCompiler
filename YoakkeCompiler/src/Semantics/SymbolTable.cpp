#include <iostream>
#include "SymbolTable.h"
#include "FunctionSymbol.h"

namespace yk
{
	SymbolTable::SymbolTable()
	{
		m_Root = new Scope();
		m_Current = m_Root;
	}

	void SymbolTable::PushScope(Scope* sc)
	{
		m_Current->Children.push_back(sc);
		sc->Parent = m_Current;
		m_Current = sc;
	}

	void SymbolTable::PopScope()
	{
		if (m_Current->Parent)
			m_Current = m_Current->Parent;
		else
			std::cout << "TRIED TO POP GLOBAL SCOPE" << std::endl;
	}

	std::vector<Symbol*>* SymbolTable::RefSymbol(std::string const& id)
	{
		return m_Current->Reference(id);
	}

	void SymbolTable::DeclSymbol(Symbol* sym)
	{
		m_Current->Declare(sym);
	}

	FunctionSymbol* SymbolTable::RefFunction(FunctionSymbol* f)
	{
		auto ls = Filter<FunctionSymbol>(RefSymbol(f->Name));
		for (auto el : ls)
		{
			if (f->Same(el))
				return el;
		}
		return nullptr;
	}
}