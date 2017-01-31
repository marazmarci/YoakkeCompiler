#include "ConstantSymbol.h"

namespace yk
{
	ConstantSymbol::ConstantSymbol(std::string const& nm, Expr* val)
		: TypedSymbol(nm, val->EvalType), Value(val)
	{
	}
}