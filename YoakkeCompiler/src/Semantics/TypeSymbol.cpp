#include "TypeSymbol.h"

namespace yk
{
	TypeSymbol::TypeSymbol(std::string const& n)
		: Symbol(n)
	{
	}

	ResolvableTypeSymbol::ResolvableTypeSymbol(std::string const& n)
		: TypeSymbol(n), Resolved(nullptr)
	{
	}

	BuiltinTypeSymbol::BuiltinTypeSymbol(std::string const& n)
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
}