#include <iostream>
#include "SemanticChecker.h"
#include "ConstantEvaluator.h"
#include "ConstantSymbol.h"
#include "VarSymbol.h"
#include "Builtin.h"
#include "../Parsing/Operator.h"
#include "../AST/TypeDesc.h"
#include "../Utils/StringUtils.h"

// TODO
// - fix flattening

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
				Check(RHS);
				Expr* exp = LHS;
				yvec<TypeSymbol*> types;
				types.push_back(RHS->EvalType);
				// Flatten
				while (true)
				{
					BinExpr* r;
					if ((r = dynamic_cast<BinExpr*>(exp)) && r->OP->OP->Symbol == ",")
					{
						Check(r->RHS);
						types.push_back(r->RHS->EvalType);
						exp = r->LHS;
					}
					else
					{
						Check(exp);
						types.insert(types.begin(), exp->EvalType);
						break;
					}
				}

				be->EvalType = new TupleTypeSymbol(types);
			}
			else if (be->OP->OP->Symbol == "=")
			{
				// TODO: better LHS handling (also for let)
				if (IdentExpr* ident = dynamic_cast<IdentExpr*>(LHS))
				{
					auto syms =
						m_Table.Filter<TypedSymbol>(m_Table.RefSymbol(ident->Ident));
					if (syms.size() == 1)
					{
						TypedSymbol* ts = syms[0];
						if (ts->Type == Builtin::UNRESOLVED)
						{
							Check(RHS);
							if (RHS->EvalType == Builtin::UNRESOLVED)
							{
								ErrorAt("Cannot use unresolved types!", RHS->Position);
							}
							else
							{
								ts->Type = RHS->EvalType;
							}
						}
						else
						{
							// Hint for RHS
							RHS->HintType = ts->Type;
							Check(RHS);
							if (!RHS->EvalType->Same(ts->Type))
							{
								ErrorAt("Specified type for assignment is not the type of given expression!",
									be->RHS->Position);
							}
						}
					}
					else if (syms.size() == 0)
					{
						ErrorAt("Undefined variable '" + ident->Ident + "'!",
							ident->Position);
					}
					else
					{
						std::cout << "SANITY ERROR" << std::endl;
					}
				}
				else
				{
					ErrorAt("Illegal left-hand side for assignment!", LHS->Position);
				}
			}
			else
			{
				Check(LHS);
				Check(RHS);
				yvec<TypeSymbol*> args = { LHS->EvalType, RHS->EvalType };

				ystr op = be->OP->OP->Symbol;
				// Check for a function
				auto tmpsyms = m_Table.Filter<ConstantSymbol>(m_Table.RefSymbol(op));
				std::vector<TypedSymbol*> syms;
				for (auto s : tmpsyms) syms.push_back(s);
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
		else if (MixfixExpr* mf = dynamic_cast<MixfixExpr*>(exp))
		{
			if (mf->OP->OP->Symbol == " fcall ")
			{
				Expr* func = mf->Operands[0];
				Expr* params = mf->Operands[1];
				yvec<TypeSymbol*> args;
				if (params)
				{
					Check(params);
				}

				FunctionTypeSymbol* fts = new FunctionTypeSymbol(args, Builtin::UNIT);
				func->HintType = fts;
				Check(func);
				mf->EvalType = ((FunctionTypeSymbol*)func->EvalType)->ReturnType;
			}
			else
				std::cout << "WAT";
		}
		else if (LetExpr* le = dynamic_cast<LetExpr*>(exp))
		{
			if (le->Type) Check(le->Type);
			if (le->Value)
			{
				if (le->Type) le->Value->HintType = le->Type->SymbolForm;
				Check(le->Value);
				if (le->Value->EvalType == Builtin::UNRESOLVED)
				{
					ErrorAt("Cannot use unresolved types!", le->Value->Position);
				}
				else
				{
					le->EvalType = le->Value->EvalType;
				}
			}
			else
				le->EvalType = Builtin::UNIT;

			// Check for type match
			if (le->Type && le->Value &&
				!le->Type->SymbolForm->Same(le->Value->EvalType))
			{
				ErrorAt("Specified type for let is not the type of given expression!",
					le->Value->Position);
			}

			// Add to symbol table
			if (IdentExpr* ident = dynamic_cast<IdentExpr*>(le->Lvalue))
			{
				auto syms =
					m_Table.Filter<TypedSymbol>(m_Table.RefSymbol(ident->Ident));
				if (syms.size())
				{
					ErrorAt("Variable '" + ident->Ident + "' already declared!",
						ident->Position);
				}
				else
				{
					if (le->Value)
						m_Table.DeclSymbol(
							new VarSymbol(ident->Ident, le->Value->EvalType));
					else if (le->Type)
						m_Table.DeclSymbol(
							new VarSymbol(ident->Ident, le->Type->SymbolForm));
					else
						m_Table.DeclSymbol(
							new VarSymbol(ident->Ident, Builtin::UNRESOLVED));
				}
			}
			else
			{
				ErrorAt("Illegal left-hand side for let!", le->Lvalue->Position);
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
		else if (FuncTypeDesc* fd = dynamic_cast<FuncTypeDesc*>(td))
		{
			yvec<TypeSymbol*> pars;
			for (auto p : fd->Parameters)
			{
				Check(p);
				pars.push_back(p->SymbolForm);
			}
			Check(fd->ReturnType);
			fd->SymbolForm = new FunctionTypeSymbol(pars, fd->ReturnType->SymbolForm);
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