#include "TypedSymbol.h"

namespace yk
{
	TypedSymbol::TypedSymbol(std::string const& nm, TypeSymbol* t)
		: Symbol(nm), Type(t)
	{
	}
}