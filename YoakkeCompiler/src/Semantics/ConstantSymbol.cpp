#include "ConstantSymbol.h"

namespace yk
{
	ConstantSymbol::ConstantSymbol(ystr const& nm, TypeSymbol* t)
		: TypedSymbol(nm, t)
	{
	}


	BuiltinConstantSymbol::BuiltinConstantSymbol(ystr const& nm, TypeSymbol* t)
		: ConstantSymbol(nm, t)
	{
	}

	UserConstantSymbol::UserConstantSymbol(ystr const& nm, Expr* e)
		: ConstantSymbol(nm, e->EvalType)
	{
	}
}