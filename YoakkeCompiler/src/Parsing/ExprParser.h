#pragma once

#include <vector>
#include "ExprElem.h"
#include "ParseState.h"
#include "Operator.h"
#include "../AST/Expr.h"

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
		std::vector<ExprElem> m_Stack;
		std::vector<ExprElemR> m_RStack;

	public:
		ExprParser(Parser& par);

	public:
		std::vector<Expr*> ParseExprList();

	private:
		void EatElements();
		void Reduce();
		std::vector<OperPred> CreateOperPred();
		std::size_t PredOperSingle(std::vector<OperPred>& preds);
		std::size_t MatchOperSingle(std::vector<OperPred>& preds);
		void CheckAmbiguity(std::vector<OperPred>& preds);
		std::vector<ExprElemR> CreateDeducedStack(std::vector<OperPred>& preds);
		double MaxPrec(std::vector<ExprElemR>& elems);
		bool ReduceOnce();
		std::size_t ReduceSingle(std::size_t idx, bool right, double prec);
		void ReducePrefixAt(std::size_t idx);
		std::size_t ReducePostfixAt(std::size_t idx);
		std::size_t ReduceInfixAt(std::size_t idx);
	};
}