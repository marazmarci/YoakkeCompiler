#include <iostream>
#include "SemanticChecker.h"
#include "ConstantEvaluator.h"
#include "ConstantSymbol.h"
#include "VarSymbol.h"
#include "../Parsing/Operator.h"
#include "../AST/TypeDesc.h"
#include "../Utils/StringUtils.h"

namespace yk
{
	SemanticChecker::SemanticChecker(const char* src)
		: m_Src(src), m_Logger(Logger("Semantics"))
	{
		m_Table.Init();
	}

	void SemanticChecker::Check(yvec<Stmt*> ls)
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
			std::cout << "UNCOVERED CASE STMT" << std::endl;
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

			auto syms = m_Table.Filter<TypedSymbol>(m_Table.RefSymbol(ie->Ident));
			ysize cnt = syms.size();
			if (cnt == 1)
			{
				typed = syms[0];
			}
			if (ie->HintType)
			{
				auto syms2 = m_Table.FilterTyped(syms, ie->HintType);
				cnt = syms2.size();
				if (cnt == 1)
				{
					typed = syms2[0];
				}
				else
					typed = nullptr;
			}

			if (typed)
			{
				ie->EvalType = typed->Type;
				ie->Sym = typed;
			}
			else if (cnt == 0)
			{
				ErrorAt("Undefined identifier: '" + ie->Ident + "'!", ie->Position);
			}
			else
			{
				ErrorAt("Identifier: '" + ie->Ident + "' is ambigous!", ie->Position);
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
			Check(fe->ReturnType);
			yvec<TypeSymbol*> params;
			for (auto p : fe->Parameters)
			{
				Check(p->Type);
				params.push_back(p->Type->SymbolForm);
			}
			fe->EvalType = new FunctionTypeSymbol(params, fe->ReturnType->SymbolForm);
		}
		else if (FuncExpr* fe = dynamic_cast<FuncExpr*>(exp))
		{
			Check(fe->Prototype);
			fe->EvalType = fe->Prototype->EvalType;
			FunctionScope* scope = new FunctionScope();
			m_Table.PushScope(scope);
			for (auto par : fe->Prototype->Parameters)
			{
				if (par->Value == "")
				{
					WarnAt("Unnamed parameter can not be referenced!", 
						par->Position);
				}
				else
				{
					auto syms = m_Table.Filter<TypedSymbol>(m_Table.RefSymbol(par->Value));
					if (syms.size())
					{
						ErrorAt("Parameter '" + par->Value + "' already declared!",
							par->Position);
					}
					else
					{
						m_Table.DeclSymbol(
							new ParamSymbol(par->Value, par->Type->SymbolForm));
					}
				}
			}
			Check(fe->Body);
			m_Table.PopScope();
		}
		else if (UryExpr* ue = dynamic_cast<UryExpr*>(exp))
		{
			std::cout << "URY is unimplemented" << std::endl;
		}
		else if (BinExpr* be = dynamic_cast<BinExpr*>(exp))
		{
			auto LHS = be->LHS;
			auto RHS = be->RHS;

			if (be->OP->OP->Symbol == "::")
			{
				if (IdentExpr* ie = dynamic_cast<IdentExpr*>(LHS))
				{
					ystr ident = ie->Ident;
					if (Expr* rval = ConstantEvaluator::Evaluate(RHS))
					{
						Check(rval);

						auto set = m_Table.RefSymbol(ident);
						auto typed = m_Table.Filter<TypedSymbol>(set);
						auto same = m_Table.FilterTyped(typed, rval->EvalType);

						if (same.empty())
						{
							m_Table.DeclSymbol(new UserConstantSymbol(ident, rval));
						}
						else
						{
							ErrorAt("Already declared (constant): '" + ie->Ident + "'!",
								ie->Position);
						}
					}
					else
					{
						ErrorAt("Right-hand side of constant binding must be a constant value!",
							be->RHS->Position);
					}
				}
				else
				{
					ErrorAt("Left-hand side of constant binding must be an identifier!",
						be->LHS->Position);
				}
			}
			else if (be->OP->OP->Symbol == ",")
			{
				Check(LHS);
				Check(RHS);
				be->EvalType = new TupleTypeSymbol(LHS->EvalType, RHS->EvalType);
			}
			else
			{
				Check(LHS);
				Check(RHS);
				yvec<TypeSymbol*> args = { LHS->EvalType, RHS->EvalType };

				ystr op = be->OP->OP->Symbol;
				// Check for a function
				auto syms = m_Table.Filter<ConstantSymbol>(m_Table.RefSymbol(op));
				auto funs = m_Table.FilterArgs(syms, args);
				if (funs.size() == 1)
				{
					be->EvalType = ((FunctionTypeSymbol*)funs[0]->Type)->ReturnType;
				}
				else if (funs.size() == 0)
				{
					ErrorAt("No operator '" + op + "' defined for operands!", be->Position);
				}
				else
				{
					std::cout << "SANITY ERROR" << std::endl;
				}
			}
		}
		else
		{
			std::cout << "UNCOVERED CASE EXPR" << std::endl;
		}
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
				ErrorAt("Undefined type '" + id->Identifier + "'!", id->Position);
			}
		}
		else
		{
			std::cout << "UNCOVERED CASE TYPE DESC" << std::endl;
		}
	}

	void SemanticChecker::ErrorAt(ystr const& msg, NodePos const& t)
	{
		ysize len = t.EndX - t.StartX;
		ysize pos = t.StartX;
		ystr ln = StringUtils::GetLine(m_Src, t.StartY);
		m_Logger.log() << "Semantic error "
			<< StringUtils::Position(t) << log::endl
			<< ln
			<< log::endl << StringUtils::GenArrow(pos, len, ln) << log::endl
			<< msg << log::endlog;
	}

	void SemanticChecker::WarnAt(ystr const& msg, NodePos const& t)
	{
		ysize len = t.EndX - t.StartX;
		ysize pos = t.StartX;
		m_Logger.log() << "Warn "
			<< StringUtils::Position(t) << log::endl
			<< msg << log::endlog;
	}
}