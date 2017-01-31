#include <iostream>
#include "SemanticChecker.h"
#include "ConstantEvaluator.h"
#include "ConstantSymbol.h"
#include "VarSymbol.h"
#include "../AST/FuncPrototype.h"
#include "../AST/TypeDesc.h"

namespace yk
{
	SemanticChecker::SemanticChecker()
	{
		m_Table.Init();
	}

	void SemanticChecker::Check(std::vector<Stmt*> ls)
	{
		for (auto s : ls)
		{
			Check(s);
		}
	}

	void SemanticChecker::Check(Stmt* st)
	{
		if (ExprStmt* es = dynamic_cast<ExprStmt*>(st))
		{
			Check(es->Sub);
		}
		else
		{
			Error("UNCOVERED CASE STMT");
		}
	}

	void SemanticChecker::Check(Expr* exp)
	{
		if (IntLiteralExpr* ie = dynamic_cast<IntLiteralExpr*>(exp))
		{
			ie->EvalType = m_Table.FilterSingle<BuiltinTypeSymbol>("i32");
		}
		else if (IdentExpr* ie = dynamic_cast<IdentExpr*>(exp))
		{
			TypedSymbol* typed = nullptr;
			std::size_t cnt = 0;
			auto syms = m_Table.Filter<TypedSymbol>(m_Table.RefSymbol(ie->Ident));
			cnt = syms.size();
			if (cnt == 1)
			{
				typed = syms[0];
			}
			else if (ie->HintType)
			{
				auto syms2 = m_Table.FilterTyped(syms, ie->HintType);
				cnt = syms2.size();
				if (cnt == 1)
				{
					typed = syms2[0];
				}
			}

			if (typed)
			{
				ie->EvalType = typed->Type;
				ie->Sym = typed;
			}
			else if (cnt == 0)
			{
				Error("Undefined identifier!");
			}
			else
			{
				Error("Identifier is ambigious");
			}
		}
		else if (BlockExpr* be = dynamic_cast<BlockExpr*>(exp))
		{
			BlockScope* scope = new BlockScope();
			m_Table.PushScope(scope);
			for (auto s : be->Statements)
			{
				Check(s);
			}
			m_Table.PopScope();
		}
		else if (FuncHeaderExpr* fe = dynamic_cast<FuncHeaderExpr*>(exp))
		{
			auto sym = CheckPrototype(fe->Prototype);
			fe->EvalType = sym;
		}
		else if (FuncExpr* fe = dynamic_cast<FuncExpr*>(exp))
		{
			auto sym = CheckPrototype(fe->Prototype);
			fe->EvalType = sym;
			FunctionScope* scope = new FunctionScope();
			m_Table.PushScope(scope);
			for (auto par : fe->Prototype->Parameters)
			{
				// TODO
				if (par.first == "")
				{
					std::cout << "Warn: Unnamed parameter can not be refeenced" << std::endl;
				}
				else
				{
					m_Table.DeclSymbol(new ParamSymbol(par.first, par.second->SymbolForm));
				}
			}
			Check(fe->Body);
			m_Table.PopScope();
		}
		else if (UryExpr* ue = dynamic_cast<UryExpr*>(exp))
		{
			Error("URY is unimplemented");
		}
		else if (BinExpr* be = dynamic_cast<BinExpr*>(exp))
		{
			auto LHS = be->LHS;
			auto RHS = be->RHS;

			if (be->OP->Symbol == "::")
			{
				if (IdentExpr* ie = dynamic_cast<IdentExpr*>(LHS))
				{
					std::string ident = ie->Ident;
					if (Expr* rval = ConstantEvaluator::Evaluate(RHS))
					{
						Check(rval);

						auto set = m_Table.RefSymbol(ident);
						auto typed = m_Table.Filter<TypedSymbol>(set);
						auto same = m_Table.FilterTyped(typed, rval->EvalType);

						if (same.empty())
						{
							m_Table.DeclSymbol(new ConstantSymbol(ident, rval));
						}
						else
						{
							Error("Already declared (constant)!");
						}
					}
					else
					{
						Error("RHS of constant binding must be a constant value!");
					}
				}
				else
				{
					Error("LHS of constant binding must be an identifier");
				}
			}
			else
			{
				Error("UNCOVERED BINOP");
			}
		}
		else
		{
			Error("UNCOVERED CASE EXPR");
		}
	}

	TypeSymbol* SemanticChecker::CheckPrototype(FuncPrototype* proto)
	{
		Check(proto->ReturnType);
		std::vector<TypeSymbol*> params;
		for (auto p : proto->Parameters)
		{
			Check(p.second);
			params.push_back(p.second->SymbolForm);
		}
		return new FunctionTypeSymbol(params, proto->ReturnType->SymbolForm);
	}

	void SemanticChecker::Check(TypeDesc* td)
	{
		if (IdentTypeDesc* id = dynamic_cast<IdentTypeDesc*>(td))
		{
			auto t = m_Table.FilterSingle<TypeSymbol>(id->Identifier);
			if (t)
			{
				id->SymbolForm = t;
			}
			else
			{
				Error("Undefined type '" + id->Identifier + "'!");
			}
		}
		else
		{
			Error("UNCOVERED CASE TYPE DESC");
		}
	}

	void SemanticChecker::Error(std::string const& err)
	{
		std::cout << "Semantic error: " << err << std::endl;
	}
}