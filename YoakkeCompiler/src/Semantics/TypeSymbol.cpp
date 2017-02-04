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

	FunctionTypeSymbol::FunctionTypeSymbol(yvec<TypeSymbol*>& params, TypeSymbol* rt)
		: TypeSymbol(""), Parameters(params), ReturnType(rt)
	{
		Name = "@f " + std::to_string(Parameters.size()) + ' ';
		for (TypeSymbol* ts : Parameters)
			Name += "@p(" + ts->Name + ") ";
		Name += "@r(" + rt->Name + ") @e";
	}

	bool FunctionTypeSymbol::Same(TypeSymbol* other)
	{
		if (FunctionTypeSymbol* ft = dynamic_cast<FunctionTypeSymbol*>(other))
		{
			if (Parameters.size() != ft->Parameters.size()) return false;
			if (!ReturnType->Same(ft->ReturnType)) return false;
			for (ysize i = 0; i < Parameters.size(); i++)
			{
				if (!Parameters[i]->Same(ft->Parameters[i])) return false;
			}
			return true;
		}
		return false;
	}

	TupleTypeSymbol::TupleTypeSymbol(yvec<TypeSymbol*> const& ls)
		: TypeSymbol(""), Types(ls)
	{
		ystr name = "@t(";
		for (auto t : ls) name += t->Name + ",";
		name += ')';
		Name = name;
	}

	bool TupleTypeSymbol::Same(TypeSymbol* other)
	{
		if (auto t = dynamic_cast<TupleTypeSymbol*>(other))
		{
			if (t->Types.size() == Types.size())
			{
				for (ysize i = 0; i < Types.size(); i++)
				{
					if (!Types[i]->Same(t->Types[i])) 
						return false;
				}
				return true;
			}
		}
		return false;
	}
}