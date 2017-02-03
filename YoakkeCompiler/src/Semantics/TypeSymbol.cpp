#include "TypeSymbol.h"

namespace yk
{
	TypeSymbol::TypeSymbol(ystr const& n)
		: Symbol(n)
	{
	}

	ResolvableTypeSymbol::ResolvableTypeSymbol(ystr const& n)
		: TypeSymbol(n), Resolved(nullptr)
	{
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
			Name += '(' + ts->Name + ") ";
		Name += "r (" + rt->Name + ")@e";
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
}