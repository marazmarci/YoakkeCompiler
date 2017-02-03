#include "VarSymbol.h"

namespace yk
{
	VarSymbol::VarSymbol(ystr const& nm, TypeSymbol* t)
		: TypedSymbol(nm, t)
	{
	}

	ParamSymbol::ParamSymbol(ystr const& nm, TypeSymbol* t)
		: VarSymbol(nm, t)
	{
	}
}