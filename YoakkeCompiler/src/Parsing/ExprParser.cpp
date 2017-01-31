#include "ExprParser.h"
#include "Parser.h"

namespace yk
{
	ExprParser::ExprParser(Parser& par)
		: m_Parser(par)
	{
	}

	std::vector<Expr*> ExprParser::ParseExprList()
	{
		EatElements();
		Reduce();

		std::vector<Expr*> ret;
		for (auto& el : m_RStack)
			ret.push_back(el.GetExpr());

		return ret;
	}

	void ExprParser::EatElements()
	{
		Operator* op = nullptr;
		Expr* exp = nullptr;
		while (true)
		{
			Token tok = m_Parser.m_CurrentToken;
			if ((op = m_Parser.GetPrefixOp(tok.Value))
				|| (op = m_Parser.GetInfixOp(tok.Value))
				|| (op = m_Parser.GetPostfixOp(tok.Value)))
			{
				m_Parser.Next();
				m_Stack.push_back(ExprElem(tok));
			}
			else if (exp = m_Parser.ParseAtom())
			{
				m_Stack.push_back(ExprElem(exp));
			}
			else
			{
				break;
			}
		}
	}

	void ExprParser::Reduce()
	{
		auto preds = CreateOperPred();
		std::size_t max = 0;
		while (PredOperSingle(preds) || MatchOperSingle(preds));
		CheckAmbiguity(preds);
		m_RStack = CreateDeducedStack(preds);

		std::size_t prevzs = m_RStack.size();
		while (true)
		{
			ReduceOnce();
			if (m_RStack.size() == prevzs)
				break;

			prevzs = m_RStack.size();
		}
	}

	std::vector<OperPred> ExprParser::CreateOperPred()
	{
		std::vector<OperPred> ret;
		ret.push_back(OperPred::Begin);
		for (std::size_t i = 0; i < m_Stack.size(); i++)
		{
			if (m_Stack[i].Tag == ExprElemT::Oper)
				ret.push_back(OperPred::Unknown);
			else
				ret.push_back(OperPred::Expr);
		}
		ret.push_back(OperPred::End);
		return ret;
	}

#define INC_CNT(x, y) (x == y ? 0 : 1)
#define INC_CNT2(x, y, z, w) (INC_CNT(x, y) + INC_CNT(z, w))
#define INC_CNT3(x, y, z, w, u, k) (INC_CNT2(x, y, z, w) + INC_CNT(u, k))
#define PREDICT(x) { cnt += INC_CNT(current, x); current = x; continue; }
#define PREDICT_N(x, y) { cnt += INC_CNT2(current, x, next, y); current = x; next = y; continue; }
#define PREDICT_P(x, y) { cnt += INC_CNT2(current, x, prev, y); current = x; prev = y; continue; }
#define PREDICT_NP(x, y, z) { cnt += INC_CNT3(current, x, next, y, prev, z); current = x; next = y; prev = z; continue; }
#define ERROR(x, y) { m_Parser.ExpectErrorAt(x, y, m_Stack[i + 1].Oper); continue; }
	std::size_t ExprParser::PredOperSingle(std::vector<OperPred>& preds)
	{
		std::size_t cnt = 0;
		for (std::size_t i = 0; i < preds.size(); i++)
		{
			auto& current = preds[i];
			if (current == OperPred::Unknown || current == OperPred::PreOrIn || current == OperPred::PostOrIn)
			{
				auto& prev = preds[i - 1];
				auto& next = preds[i + 1];

				if (prev == OperPred::Begin)	PREDICT(OperPred::Prefx);
				if (next == OperPred::End)		PREDICT(OperPred::Postfx);

				if (prev == OperPred::Expr)
				{
					if (next == OperPred::Expr)		PREDICT(OperPred::Infx);
					if (next == OperPred::Prefx)	PREDICT(OperPred::Infx);
					if (next == OperPred::Infx)		PREDICT(OperPred::Postfx);
					if (next == OperPred::PreOrIn)	PREDICT(OperPred::PostOrIn);
					if (next == OperPred::PostOrIn)	PREDICT(OperPred::Postfx);
					if (next == OperPred::Unknown)	PREDICT(OperPred::PostOrIn);

					if (next == OperPred::Postfx)
						ERROR("Prefix or infix operator", "postfix");
				}
				if (prev == OperPred::Prefx)
				{
					if (next == OperPred::Expr)		PREDICT(OperPred::Prefx);
					if (next == OperPred::Prefx)	PREDICT(OperPred::Prefx);
					if (next == OperPred::PreOrIn)	PREDICT_N(OperPred::Prefx, OperPred::Prefx);
					if (next == OperPred::Unknown)	PREDICT(OperPred::Prefx);

					if (next == OperPred::Infx)
						ERROR("Prefix operator", "infix");

					if (next == OperPred::Postfx)
						ERROR("Prefix operator", "postfix");

					if (next == OperPred::PostOrIn)
						ERROR("Prefix operator", "postfix or infix");
				}
				if (prev == OperPred::Infx)
				{
					if (next == OperPred::Expr)		PREDICT(OperPred::Prefx);
					if (next == OperPred::Prefx)	PREDICT(OperPred::Prefx);
					if (next == OperPred::PreOrIn)	PREDICT_N(OperPred::Prefx, OperPred::Prefx);
					if (next == OperPred::Unknown)	PREDICT(OperPred::Prefx);

					if (next == OperPred::Infx)
						ERROR("Prefix operator", "infix");

					if (next == OperPred::Postfx)
						ERROR("Prefix operator", "postfix");

					if (next == OperPred::PostOrIn)
						ERROR("Prefix operator", "postfix or infix");
				}
				if (prev == OperPred::Postfx)
				{
					if (next == OperPred::Expr)		PREDICT(OperPred::Infx);
					if (next == OperPred::Prefx)	PREDICT(OperPred::Infx);
					if (next == OperPred::Infx)		PREDICT(OperPred::Postfx);
					if (next == OperPred::Postfx)	PREDICT(OperPred::Infx);
					if (next == OperPred::PreOrIn)	PREDICT(OperPred::PostOrIn);
					if (next == OperPred::PostOrIn)	PREDICT(OperPred::Postfx);
					if (next == OperPred::Unknown)	PREDICT(OperPred::PostOrIn);
				}
				if (prev == OperPred::PreOrIn)
				{
					if (next == OperPred::Expr)		PREDICT(OperPred::Prefx);
					if (next == OperPred::Prefx)	PREDICT(OperPred::Prefx);
					if (next == OperPred::PreOrIn)	PREDICT(OperPred::Prefx);
					if (next == OperPred::Unknown)	PREDICT(OperPred::Prefx);

					if (next == OperPred::Infx)
						ERROR("Prefix operator", "infix");

					if (next == OperPred::Postfx)
						ERROR("Prefix operator", "postfix");

					if (next == OperPred::PostOrIn)
						ERROR("Prefix operator", "postfix or infix");
				}
				if (prev == OperPred::PostOrIn)
				{
					if (next == OperPred::Expr)		PREDICT(OperPred::PreOrIn);
					if (next == OperPred::Prefx)	PREDICT(OperPred::PreOrIn);
					if (next == OperPred::Infx)		PREDICT_P(OperPred::Postfx, OperPred::Postfx);
					if (next == OperPred::Postfx)	PREDICT_P(OperPred::Postfx, OperPred::Postfx);
					//if (next == OperPred::PreOrIn)	ERROR("AMBIGUITY");
					//if (next == OperPred::PostOrIn)	ERROR("AMBIGUITY");
					//if (next == OperPred::Unknown)	PREDICT(OperPred::PreOrIn);
				}
				if (prev == OperPred::Unknown)
				{
					if (next == OperPred::Expr)		PREDICT(OperPred::PreOrIn);
					if (next == OperPred::Prefx)	PREDICT(OperPred::PreOrIn);
					if (next == OperPred::Infx)		PREDICT_P(OperPred::Postfx, OperPred::Postfx);
					if (next == OperPred::Postfx)	PREDICT_P(OperPred::Postfx, OperPred::Postfx);
					//if (next == OperPred::PreOrIn)	ERROR("AMBIGUITY");
					if (next == OperPred::PostOrIn)	PREDICT_NP(OperPred::Postfx, OperPred::Postfx, OperPred::Postfx);
					//if (next == OperPred::Unknown)	ERROR("AMBIGUITY");
				}
			}
		}
		return cnt;
	}
#undef INC_CNT(x, y)
#undef INC_CNT2(x, y, z, w)
#undef INC_CNT3(x, y, z, w, u, k)
#undef PREDICT(x)
#undef PREDICT_N(x, y)
#undef PREDICT_P(x, y)
#undef PREDICT_NP(x, y, z)
#undef ERROR(x)

#define PREDICT(x) { pred = x; cnt++; }
	std::size_t ExprParser::MatchOperSingle(std::vector<OperPred>& preds)
	{
		std::size_t cnt = 0;
		for (std::size_t idx = 0; idx < m_Stack.size(); idx++)
		{
			auto& pred = preds[idx + 1];	// Begin is extra
			if (pred == OperPred::Begin || pred == OperPred::End || pred == OperPred::Expr)
				continue;
			Token* tok = m_Stack[idx].GetOper();
			std::string sym = tok->Value;
			Operator* prefx = m_Parser.GetPrefixOp(sym);
			Operator* infx = m_Parser.GetInfixOp(sym);
			Operator* postfx = m_Parser.GetPostfixOp(sym);

			switch (pred)
			{
			case OperPred::Prefx:
				if (!prefx) 
					m_Parser.ErrorAt("Operator mismatch (prefix operator expected)!",
						m_Stack[idx].Oper);
				break;

			case OperPred::Infx:
				if (!infx)
					m_Parser.ErrorAt("Operator mismatch (infix operator expected)!",
						m_Stack[idx].Oper);
				break;

			case OperPred::Postfx:
				if (!postfx)
					m_Parser.ErrorAt("Operator mismatch (postfix operator expected)!",
						m_Stack[idx].Oper);
				break;

			case OperPred::PostOrIn:
				if (!postfx && infx) PREDICT(OperPred::Infx);
				if (postfx && !infx) PREDICT(OperPred::Postfx);
				if (!postfx && !infx)
					m_Parser.ErrorAt("Operator mismatch (postfix or infix operator expected)!",
						m_Stack[idx].Oper);
				break;

			case OperPred::PreOrIn:
				if (!prefx && infx) PREDICT(OperPred::Infx);
				if (prefx && !infx) PREDICT(OperPred::Prefx);
				if (!prefx && !infx)
					m_Parser.ErrorAt("Operator mismatch (prefix or infix operator expected)!",
						m_Stack[idx].Oper);
				break;

			case OperPred::Unknown:
				if (prefx && !infx && !postfx) PREDICT(OperPred::Prefx);
				if (!prefx && infx && !postfx) PREDICT(OperPred::Infx);
				if (!prefx && !infx && postfx) PREDICT(OperPred::Postfx);

				if (prefx && infx && !postfx) PREDICT(OperPred::PreOrIn);
				if (!prefx && infx && postfx) PREDICT(OperPred::PostOrIn);

				break;
			}
		}
		return cnt;
	}
#undef PREDICT(x)

	void ExprParser::CheckAmbiguity(std::vector<OperPred>& preds)
	{
		for (std::size_t idx = 0; idx < m_Stack.size(); idx++)
		{
			auto& pred = preds[idx + 1];	// Begin is extra
			if (pred == OperPred::Begin || pred == OperPred::End || pred == OperPred::Expr)
				continue;

			if (pred != OperPred::Prefx && pred != OperPred::Infx && pred != OperPred::Postfx)
			{
				m_Parser.ErrorAt("Operator ambiguity!", m_Stack[idx].Oper);
			}
		}
	}

	std::vector<ExprElemR> ExprParser::CreateDeducedStack(std::vector<OperPred>& preds)
	{
		std::vector<ExprElemR> ret;
		for (std::size_t i = 0; i < m_Stack.size(); i++)
		{
			auto& pred = preds[i + 1];
			if (pred == OperPred::Expr) 
				ret.push_back(ExprElemR(m_Stack[i].GetExpr()));
			else
			{
				Token* tok = m_Stack[i].GetOper();
				if (pred == OperPred::Prefx)
					ret.push_back(ExprElemR(m_Parser.GetPrefixOp(tok->Value), *tok));
				if (pred == OperPred::Infx)
					ret.push_back(ExprElemR(m_Parser.GetInfixOp(tok->Value), *tok));
				if (pred == OperPred::Postfx)
					ret.push_back(ExprElemR(m_Parser.GetPostfixOp(tok->Value), *tok));
			}
		}
		return ret;
	}

	Operator* ExprParser::MaxPrec(std::vector<ExprElemR>& elems)
	{
		Operator* maxop = nullptr;
		double max = -1.0;
		Operator* op;
		for (auto& el : elems)
		{
			if ((op = el.GetOper()) && op->Precedence > max)
			{
				max = op->Precedence;
				maxop = op;
			}
		}
		return maxop;
	}

	bool ExprParser::ReduceOnce()
	{
		Operator* maxpr = MaxPrec(m_RStack);
		if (maxpr)
		{
			if (UryOp* uryop = dynamic_cast<UryOp*>(maxpr))
			{
				if (uryop->Fixity == FixityT::Prefix) ReducePrefix(uryop);
				if (uryop->Fixity == FixityT::Postfix) ReducePostfix(uryop);
			}
			if (BinOp* binop = dynamic_cast<BinOp*>(maxpr))
			{
				ReduceInfix(binop);
			}
			return true;
		}
		return false;
	}

	void ExprParser::ReducePrefix(UryOp* op)
	{
		for (std::size_t i = 0; i < m_RStack.size(); i++)
		{
			auto& elem = m_RStack[i];
			if (elem.GetOper() == op)
			{
				ReducePrefixAt(i);
			}
		}
	}

	void ExprParser::ReducePrefixAt(std::size_t idx)
	{
		auto begop = m_RStack[idx];
		Expr* exp = nullptr;
		int j;
		for (j = idx; j < m_RStack.size(); j++)
		{
			if (exp = m_RStack[j].GetExpr())
			{
				break;
			}
		}
		if (exp)
		{
			std::size_t cnt = 0;
			for (j--; (int)idx <= j; j--)
			{
				cnt++;
				exp = new UryExpr(exp, (UryOp*)m_RStack[j].GetOper());
			}
			m_RStack.erase(m_RStack.begin() + idx, m_RStack.begin() + idx + cnt + 1);
			if (idx == m_RStack.size())
				m_RStack.push_back(exp);
			else
				m_RStack.insert(m_RStack.begin() + idx, exp);
		}
		else
		{
			m_Parser.ErrorAt("No operand for prefix operator", begop.Reference);
		}
	}

	void ExprParser::ReducePostfix(UryOp* op)
	{
		for (std::size_t i = 0; i < m_RStack.size(); i++)
		{
			auto& elem = m_RStack[i];
			if (elem.GetOper() == op)
			{
				i -= ReducePostfixAt(i);
			}
		}
	}

	std::size_t ExprParser::ReducePostfixAt(std::size_t idx)
	{
		auto begop = m_RStack[idx];
		Expr* exp = nullptr;
		int j;
		for (j = idx; j >= 0; j--)
		{
			if (m_RStack[j].GetExpr())
			{
				exp = m_RStack[j].GetExpr();
				break;
			}
		}
		if (exp)
		{
			std::size_t cnt = 0;
			for (j++; j <= idx; j++)
			{
				cnt++;
				exp = new UryExpr(exp, (UryOp*)m_RStack[j].GetOper());
			}
			m_RStack.erase(m_RStack.begin() + (idx - cnt), m_RStack.begin() + idx + 1);
			if (idx - cnt == m_RStack.size())
				m_RStack.push_back(exp);
			else
				m_RStack.insert(m_RStack.begin() + (idx - cnt), exp);

			return cnt;
		}
		else
		{
			m_Parser.ErrorAt("No operand for postfix operator", begop.Reference);
		}
		return 0;
	}

	void ExprParser::ReduceInfix(BinOp* op)
	{
		int dir = op->Assoc == AssocT::Left ? 1 : -1;
		int start = op->Assoc == AssocT::Left ? 0 : (m_RStack.size() - 1);
		int last = -3;

		for (int i = start; i >= 0 && i < m_RStack.size(); i += dir)
		{
			auto& elem = m_RStack[i];
			if (elem.GetOper() == op)
			{
				if (op->Assoc == AssocT::Noassoc && std::abs(last - i) == 2)
				{
					m_Parser.ErrorAt("Cannot chain non-associative operators!", elem.Reference);
				}
				last = i;
				i -= ReducePostfixAt(i - 1);
				ReducePrefixAt(i + 1);
				Expr* exp =
					new BinExpr(m_RStack[i - 1].GetExpr(), m_RStack[i + 1].GetExpr(), op);

				m_RStack.erase(m_RStack.begin() + (i - 1), m_RStack.begin() + (i + 2));
				if (i - 1 == m_RStack.size())
					m_RStack.push_back(exp);
				else
					m_RStack.insert(m_RStack.begin() + (i - 1), exp);
			}
		}
	}
}
