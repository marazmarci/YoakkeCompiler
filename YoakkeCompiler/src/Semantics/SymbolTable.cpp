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
		yvec<TypeSymbol*> pars = { par, par };
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
		for (auto s : syms)
		{
			if (s->Type->Same(match))
				res.push_back(s);
		}
		return res;
	}

	yvec<ConstantSymbol*> SymbolTable::FilterArgs(yvec<ConstantSymbol*>& syms, yvec<TypeSymbol*>& args)
	{
		yvec<ConstantSymbol*> ret;
		for (auto s : syms)
		{
			if (auto* ft = dynamic_cast<FunctionTypeSymbol*>(s->Type))
			{
				if (ft->Parameters.size() == args.size())
				{
					bool match = true;
					for (ysize i = 0; i < args.size(); i++)
					{
						if (!ft->Parameters[i]->Same(args[i]))
						{
							match = false;
							break;
						}
					}
					if (match)
						ret.push_back(s);
				}
			}
		}
		return ret;
	}
}