#include "EnclosedExpr.h"

namespace yk
{
	namespace Enclosed
	{
		bool EncloseHere(yvec<ExprElemR>& stack, ysize idx, ystr const& l, ystr const& r)
		{
			if (EnclosedExpr* ee = dynamic_cast<EnclosedExpr*>(stack[idx].GetExpr()))
			{
				return (ee->Begin.Value == l && ee->End.Value == r);
			}
			return false;
		}

		bool FuncCallHere(yvec<ExprElemR>& stack, ysize idx)
		{
			if (idx < stack.size() - 1 && stack[idx].GetExpr())
			{
				return (EncloseHere(stack, idx + 1, "(", ")"));
			}
			return false;
		}

		ysize FuncCallReduce(yvec<ExprElemR>& stack, ysize idx, Operator* op)
		{
			yvec<Expr*> args;
			Expr* fun = stack[idx].GetExpr();
			Expr* par = stack[idx + 1].GetExpr();
			Expr* pars = ((EnclosedExpr*)par)->Sub;

			args.push_back(fun);
			args.push_back(pars);
			Expr* fcall = 
				new MixfixExpr(args, 
					new OperExpr(op, Token(TokenT::Epsilon, "", 0, 0)), 
					NodePos::Join(fun, par));
			stack.erase(stack.begin() + idx, stack.begin() + (idx + 2));
			if (idx == stack.size())
				stack.push_back(fcall);
			else
				stack.insert(stack.begin() + idx, fcall);
			return 1;
		}
	}
}