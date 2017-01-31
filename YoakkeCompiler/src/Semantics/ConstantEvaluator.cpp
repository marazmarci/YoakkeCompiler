#include "ConstantEvaluator.h"

namespace yk
{
	namespace ConstantEvaluator
	{
		Expr* Evaluate(Expr* exp)
		{
			if (dynamic_cast<FuncExpr*>(exp)
				|| dynamic_cast<IntLiteralExpr*>(exp)
				|| dynamic_cast<FuncHeaderExpr*>(exp)
				)
				return exp;

			return nullptr;
		}
	}
}