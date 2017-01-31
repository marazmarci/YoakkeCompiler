#include <iostream>
#include "SemanticChecker.h"
#include "ConstantEvaluator.h"
#include "ConstantSymbol.h"
#include "VarSymbol.h"
#include "../AST/FuncPrototype.h"
#include "../AST/TypeDesc.h"
#include "../Utils/StringUtils.h"

namespace yk
{
	SemanticChecker::SemanticChecker(const char* src)
		: m_Src(src), m_Logger(Logger("Semantics"))
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
				ErrorAt("Undefined identifier: '" + ie->Ident + "'!", ie->Reference);
			}
			else
			{
				ErrorAt("Identifier: '" + ie->Ident + "' is ambigous!", ie->Reference);
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
			std::cout << "URY is unimplemented" << std::endl;
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
							ErrorAt("Already declared (constant): '" + ie->Ident + "'!",
								ie->Reference);
						}
					}
					else
					{
						ErrorAt("Right-hand side of constant binding must be a constant value!",
							be->Reference);
					}
				}
				else
				{
					ErrorAt("Left-hand side of constant binding must be an identifier!",
						be->Reference);
				}
			}
			else
			{
				std::cout << "UNCOVERED BINOP" << std::endl;
			}
		}
		else
		{
			std::cout << "UNCOVERED CASE EXPR" << std::endl;
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
				ErrorAt("Undefined type '" + id->Identifier + "'!", id->Reference);
			}
		}
		else
		{
			std::cout << "UNCOVERED CASE TYPE DESC" << std::endl;
		}
	}

	void SemanticChecker::ErrorAt(std::string const& msg, Token const& t)
	{
		std::size_t len = t.Value.size();
		std::size_t pos = t.Column - len;
		m_Logger.log() << "Syntax error "
			<< StringUtils::Position(t) << log::endl
			<< StringUtils::GetLine(m_Src, t.Row)
			<< log::endl << StringUtils::GenArrow(pos, len) << log::endl
			<< msg << log::endlog;
	}
}