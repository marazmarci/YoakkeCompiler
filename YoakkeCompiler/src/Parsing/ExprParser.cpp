#include "ExprParser.h"
#include "Parser.h"

namespace yk
{
	ExprParser::ExprParser(Parser& par, ystr const& stop)
		: m_Parser(par), m_Stop(stop)
	{
	}

	yvec<Expr*> ExprParser::ParseExprList()
	{
		EatElements();
		Reduce();

		yvec<Expr*> ret;
		for (auto& el : m_RStack)
			ret.push_back(el.GetExpr());

		return ret;
	}

	void ExprParser::EatElements()
	{
		Operator* op = nullptr;
		Expr* exp = nullptr;
		if (m_Stop == "")
		{
			while (true)
			{
				if (!EatSingle())
					break;
			}
		}
		else
		{
			while (m_Parser.m_CurrentToken.Value != m_Stop)
			{
				if (!EatSingle())
					break;
			}
		}
	}

	bool ExprParser::EatSingle()
	{
		Operator* op = nullptr;
		Expr* exp = nullptr;

		Token tok = m_Parser.m_CurrentToken;
		if ((op = m_Parser.GetPrefixOp(tok.Value))
			|| (op = m_Parser.GetInfixOp(tok.Value))
			|| (op = m_Parser.GetPostfixOp(tok.Value)))
		{
			m_Parser.Next();
			m_Stack.push_back(ExprElem(tok));
			return true;
		}
		if (exp = m_Parser.ParseAtom())
		{
			m_Stack.push_back(ExprElem(exp));
			return true;
		}
		return false;
	}

	void ExprParser::Reduce()
	{
		auto preds = CreateOperPred();
		ysize max = 0;
		while (PredOperSingle(preds) || MatchOperSingle(preds));
		CheckAmbiguity(preds);
		m_RStack = CreateDeducedStack(preds);

		ysize prevzs = m_RStack.size();
		while (true)
		{
			ReduceOnce();
			if (m_RStack.size() == prevzs)
				break;

			prevzs = m_RStack.size();
		}
	}

	yvec<OperPred> ExprParser::CreateOperPred()
	{
		yvec<OperPred> ret;
		ret.push_back(OperPred::Begin);
		for (ysize i = 0; i < m_Stack.size(); i++)
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
	ysize ExprParser::PredOperSingle(yvec<OperPred>& preds)
	{
		ysize cnt = 0;
		for (ysize i = 0; i < preds.size(); i++)
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
	ysize ExprParser::MatchOperSingle(yvec<OperPred>& preds)
	{
		ysize cnt = 0;
		for (ysize idx = 0; idx < m_Stack.size(); idx++)
		{
			auto& pred = preds[idx + 1];	// Begin is extra
			if (pred == OperPred::Begin || pred == OperPred::End || pred == OperPred::Expr)
				continue;
			Token* tok = m_Stack[idx].GetOper();
			ystr sym = tok->Value;
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

	void ExprParser::CheckAmbiguity(yvec<OperPred>& preds)
	{
		for (ysize idx = 0; idx < m_Stack.size(); idx++)
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

	yvec<ExprElemR> ExprParser::CreateDeducedStack(yvec<OperPred>& preds)
	{
		yvec<ExprElemR> ret;
		for (ysize i = 0; i < m_Stack.size(); i++)
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

	double ExprParser::MaxPrec()
	{
		double max = -1.0;
		Operator* op;
		for (auto& el : m_RStack)
		{
			if ((op = el.GetOper()) && op->Precedence > max)
				max = op->Precedence;
		}
		// Try mixfix operators
		for (auto mf : m_Parser.m_MixfixOps)
		{
			if (mf.Precedence > max)
			{
				for (ysize i = 0; i < m_RStack.size(); i++)
				{
					if (mf.Here(m_RStack, i))
					{
						max = mf.Precedence;
						break;
					}
				}
			}
		}
		return max;
	}

	bool ExprParser::ReduceOnce()
	{
		double mp = MaxPrec();
		if (mp >= 0)
		{
			// Check for noassoc sanity
			for (int i = 0; i < m_RStack.size() - 2; i++)
			{
				BinOp* aop;
				BinOp* bop;
				if ((aop = dynamic_cast<BinOp*>(m_RStack[i].GetOper())) &&
					(bop = dynamic_cast<BinOp*>(m_RStack[i + 2].GetOper())) &&
					aop->Assoc == AssocT::Noassoc && bop->Assoc == AssocT::Noassoc)
				{
					m_Parser.ErrorAt("Cannot chain non-associative operators!", m_RStack[i + 2].Reference);
				}
			}

			// Left-to-right
			for (ysize i = 0; i < m_RStack.size(); i++)
			{
				i -= ReduceSingle(i, false, mp);
			}
			// Right-to-right
			for (int i = m_RStack.size() - 1; i >= 0; i--)
			{
				 i-= ReduceSingle(i, true, mp);
			}
			return true;
		}
		return false;
	}

	ysize ExprParser::ReduceSingle(ysize idx, bool right, double prec)
	{
		Operator* op = m_RStack[idx].GetOper();
		ysize offs = 0;
		if (op && op->Precedence == prec)
		{
			if (UryOp* uryop = dynamic_cast<UryOp*>(op))
			{
				if (!right)
				{
					if (uryop->Fixity == FixityT::Prefix) ReducePrefixAt(idx);
					if (uryop->Fixity == FixityT::Postfix) offs = ReducePostfixAt(idx);
				}
			}
			if (BinOp* binop = dynamic_cast<BinOp*>(op))
			{
				if (binop->Assoc == AssocT::Left && !right) offs = ReduceInfixAt(idx);
				if (binop->Assoc == AssocT::Right && right) offs = ReduceInfixAt(idx);

				if (binop->Assoc == AssocT::Noassoc && !right) offs = ReduceInfixAt(idx);
			}
		}
		else if (!right)
		{
			for (auto& mf : m_Parser.m_MixfixOps)
			{
				if (mf.Precedence == prec && mf.Here(m_RStack, idx))
					offs = mf.Reduce(m_RStack, idx, &mf);
			}
		}
		return offs;
	}

	void ExprParser::ReducePrefixAt(ysize idx)
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
			ysize cnt = 0;
			for (j--; (int)idx <= j; j--)
			{
				cnt++;
				Token const& tok = m_RStack[j].Reference;
				exp = new UryExpr(exp, 
					new OperExpr(m_RStack[j].GetOper(), m_RStack[j].Reference));
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

	ysize ExprParser::ReducePostfixAt(ysize idx)
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
			ysize cnt = 0;
			for (j++; j <= idx; j++)
			{
				cnt++;
				exp = new UryExpr(exp, 
					new OperExpr(m_RStack[j].GetOper(), m_RStack[j].Reference));
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

	ysize ExprParser::ReduceInfixAt(ysize idx)
	{
		ysize ret = ReducePostfixAt(idx - 1);
		ReducePrefixAt(idx + 1);

		Token const& tok = m_RStack[idx].Reference;
		BinOp* op = (BinOp*)m_RStack[idx].GetOper();
		Expr* exp = new BinExpr(m_RStack[idx - 1].GetExpr(), 
								m_RStack[idx + 1].GetExpr(), 
								new OperExpr(op, tok));

		m_RStack.erase(m_RStack.begin() + (idx - 1), m_RStack.begin() + (idx + 2));
		if (idx - 1 == m_RStack.size())
			m_RStack.push_back(exp);
		else
			m_RStack.insert(m_RStack.begin() + (idx - 1), exp);

		return ret + 1;
	}
}
