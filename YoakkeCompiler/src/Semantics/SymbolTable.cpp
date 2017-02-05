#include <iostream>
#include "SymbolTable.h"
#include "ConstantSymbol.h"
#include "TypedSymbol.h"
#include "Builtin.h"

namespace yk
{
	SymbolTable::SymbolTable()
	{
		m_Root = new Scope();
		m_Current = m_Root;
	}

	static BuiltinConstantSymbol* make_oper(ystr const& n, TypeSymbol* par, TypeSymbol* ret)
	{
		TypeSymbol* pars = new TupleTypeSymbol(par, par);
		FunctionTypeSymbol* type = new FunctionTypeSymbol(pars, ret);
		return new BuiltinConstantSymbol(n, type);
	}

	void SymbolTable::Init()
	{
		DeclSymbol(Builtin::UNIT = new BuiltinTypeSymbol("unit"));
		DeclSymbol(Builtin::I32 = new BuiltinTypeSymbol("i32"));

		DeclSymbol(make_oper("+", Builtin::I32, Builtin::I32));
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
		if (FunctionTypeSymbol* ft = dynamic_cast<FunctionTypeSymbol*>(match))
		{
			return FilterArgs(syms, ft->Parameters);
		}
		else
		{
			for (auto s : syms)
			{
				if (s->Type->Same(match))
					res.push_back(s);
			}
		}
		return res;
	}

	yvec<TypedSymbol*> SymbolTable::FilterArgs(yvec<TypedSymbol*>& syms, TypeSymbol* args)
	{
		yvec<TypedSymbol*> ret;
		for (auto s : syms)
		{
			if (auto* ft = dynamic_cast<FunctionTypeSymbol*>(s->Type))
			{
				if (!ft->Parameters && !args)
					ret.push_back(s);
				else if (!ft->Parameters || !args)
				{
				}
				else if (ft->Parameters->Same(args))
					ret.push_back(s);
			}
		}
		return ret;
	}
}