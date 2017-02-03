#include "ConstantSymbol.h"

namespace yk
{
	ConstantSymbol::ConstantSymbol(ystr const& nm, Expr* val)
		: TypedSymbol(nm, val->EvalType), Value(val)
	{
	}
}