#include "VarSymbol.h"

namespace yk
{
	VarSymbol::VarSymbol(std::string const& nm, TypeSymbol* t)
		: TypedSymbol(nm, t)
	{
	}

	ParamSymbol::ParamSymbol(std::string const& nm, TypeSymbol* t)
		: VarSymbol(nm, t)
	{
	}
}