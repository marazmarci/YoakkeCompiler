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

	public:
		ExprParser(Parser& par);

	public:
		std::vector<Expr*> ParseExprList();

	private:
		void EatElements();
		void Reduce(std::size_t from, std::size_t to);
		std::vector<OperPred> CreateOperPred(std::size_t from, std::size_t to);
		std::size_t PredOperSingle(std::vector<OperPred>& preds);
		std::size_t MatchOperSingle(std::vector<OperPred>& preds);
		void CheckAmbiguity(std::vector<OperPred>& preds);
	};
}