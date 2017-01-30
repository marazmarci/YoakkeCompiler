#include "FunctionSymbol.h"
#include "TypeSymbol.h"

namespace yk
{
	ParamSymbol::ParamSymbol(std::string const& n, TypeSymbol* t)
		: Symbol(n), Type(t)
	{
	}

	FunctionSymbol::FunctionSymbol(std::string const& n, TypeSymbol* ret)
		: Symbol(n), Scope(), ReturnType(ret)
	{
	}

	bool FunctionSymbol::Same(FunctionSymbol* other)
	{
		if (Name != other->Name) return false;
		if (Parameters.size() != other->Parameters.size()) return false;
		if (!ReturnType->Same(other->ReturnType)) return false;

		for (std::size_t i = 0; i < Parameters.size(); i++)
		{
			if (!Parameters[i]->Type->Same(other->Parameters[i]->Type)) return false;
		}
		return true;
	}
}