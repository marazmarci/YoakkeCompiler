#include "TypeSymbol.h"

namespace yk
{
	TypeSymbol::TypeSymbol(ystr const& n)
		: Symbol(n)
	{
	}

	UnresolvedTypeSymbol::UnresolvedTypeSymbol()
		: TypeSymbol("@unresolved")
	{
	}

	bool UnresolvedTypeSymbol::Same(TypeSymbol* other)
	{
		return false;
	}

	BuiltinTypeSymbol::BuiltinTypeSymbol(ystr const& n)
		: TypeSymbol(n)
	{
	}

	bool BuiltinTypeSymbol::Same(TypeSymbol* other)
	{
		if (BuiltinTypeSymbol* bi = dynamic_cast<BuiltinTypeSymbol*>(other))
		{
			return Name == other->Name;
		}
		return false;
	}

	FunctionTypeSymbol::FunctionTypeSymbol(TypeSymbol* params, TypeSymbol* rt)
		: TypeSymbol("@f@p(" + (params ? params->Name : "") + ")@r(" + rt->Name + ')'), 
		Parameters(params), ReturnType(rt)
	{
	}

	bool FunctionTypeSymbol::Same(TypeSymbol* other)
	{
		if (FunctionTypeSymbol* ft = dynamic_cast<FunctionTypeSymbol*>(other))
		{
			if (ReturnType->Same(ft->ReturnType))
			{
				if (!ft->Parameters && !Parameters) return true;
				if (Parameters->Same(ft->Parameters)) return true;
			}
		}
		return false;
	}

	TupleTypeSymbol::TupleTypeSymbol(TypeSymbol* l, TypeSymbol* r)
		: TypeSymbol("@t(" + l->Name + ',' + r->Name + ')'), LHS(l), RHS(r)
	{
	}

	bool TupleTypeSymbol::Same(TypeSymbol* other)
	{
		if (auto t = dynamic_cast<TupleTypeSymbol*>(other))
		{
			return LHS->Same(t->LHS) && RHS->Same(t->RHS);
		}
		return false;
	}
}