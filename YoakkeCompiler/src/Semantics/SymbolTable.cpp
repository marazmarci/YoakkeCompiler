#include <iostream>
#include "SymbolTable.h"
#include "ConstantSymbol.h"
#include "TypedSymbol.h"

namespace yk
{
	SymbolTable::SymbolTable()
	{
		m_Root = new Scope();
		m_Current = m_Root;
	}

	void SymbolTable::Init()
	{
		DeclSymbol(new BuiltinTypeSymbol("unit"));
		DeclSymbol(new BuiltinTypeSymbol("i32"));
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

	yvec<Symbol*>* SymbolTable::RefSymbol(ystr const& id)
	{
		return m_Current->Reference(id);
	}

	void SymbolTable::DeclSymbol(Symbol* sym)
	{
		m_Current->Declare(sym);
	}

	yvec<TypedSymbol*> SymbolTable::FilterTyped(yvec<TypedSymbol*>& syms, TypeSymbol* match)
	{
		yvec<TypedSymbol*> res;
		for (auto s : syms)
		{
			if (s->Type->Same(match))
				res.push_back(s);
		}
		return res;
	}
}