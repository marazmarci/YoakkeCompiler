#pragma once

#include "../AST/Expr.h"

namespace yk
{
	namespace ConstantEvaluator
	{
		Expr* Evaluate(Expr* exp);
	}
}