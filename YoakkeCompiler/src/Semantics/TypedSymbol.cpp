#include "TypedSymbol.h"

namespace yk
{
	TypedSymbol::TypedSymbol(ystr const& nm, TypeSymbol* t)
		: Symbol(nm), Type(t)
	{
	}
}