#pragma once

#include <vector>
#include "ExprElem.h"
#include "ParseState.h"
#include "Operator.h"
#include "../AST/Expr.h"
#include "../Types.h"

namespace yk
{
	class Parser;

	enum class OperPred
	{
		Begin, End, Expr, Prefx, Infx, Postfx, Unknown,
		PostOrIn, PreOrIn
	};

	class ExprParser
	{
	private:
		Parser& m_Parser;
		yvec<ExprElem> m_Stack;
		yvec<ExprElemR> m_RStack;
		ystr m_Stop;

	public:
		ExprParser(Parser& par, ystr const& stop);

	public:
		yvec<Expr*> ParseExprList();

	private:
		void EatElements();
		bool EatSingle();
		void Reduce();
		yvec<OperPred> CreateOperPred();
		ysize PredOperSingle(yvec<OperPred>& preds);
		ysize MatchOperSingle(yvec<OperPred>& preds);
		void CheckAmbiguity(yvec<OperPred>& preds);
		yvec<ExprElemR> CreateDeducedStack(yvec<OperPred>& preds);
		double MaxPrec(yvec<ExprElemR>& elems);
		bool ReduceOnce();
		ysize ReduceSingle(ysize idx, bool right, double prec);
		void ReducePrefixAt(ysize idx);
		ysize ReducePostfixAt(ysize idx);
		ysize ReduceInfixAt(ysize idx);
	};
}