#pragma once

#include <vector>
#include "Lexer.h"
#include "OperDesc.h"
#include "ParseState.h"
#include "../AST/Expr.h"
#include "../AST/FuncPrototype.h"
#include "../AST/Stmt.h"
#include "../AST/TypeDesc.h"

namespace yk
{
	enum class StackElemT
	{
		Expr, Oper, Save
	};

	class StackElem
	{
	public:
		StackElemT Tag;
		ParseState State;
		void* Data;

	public:
		StackElem(OperDesc* o, ParseState st)
			: Tag(StackElemT::Oper), Data(o), State(st)
		{
		}

		StackElem(Expr* e, ParseState st)
			: Tag(StackElemT::Expr), Data(e), State(st)
		{
		}

		StackElem(ParseState st)
			: Tag(StackElemT::Save), Data(nullptr), State(st)
		{
		}

		template <typename T>
		inline T* Get()
		{
			return (T*)Data;
		}
	};

	class Parser
	{
	private:
		yk::Lexer m_Lexer;
		Token m_CurrentToken;
		std::map<std::string, OperDesc> m_BinOps;

	public:
		Parser();

	private:
		void Error(std::string const& msg);

		void Next();
		bool Same(std::string const& val);
		bool Match(std::string const& val);
		bool IsIdent();
		std::string const& GetValue();
		
		ParseState SaveState();
		void LoadState(ParseState const& st);

		void AddOperator(OperDesc desc);
		OperDesc* GetBinOp(Token t);

	public:
		std::vector<Stmt*> ParseProgram(const char* src);

	private:
		Stmt* ParseStmt();

		Stmt* ParseExprStmt();

		ParamPair ParseParameter();
		FuncPrototype* ParseFuncPrototype();
		Expr* ParseExpr();
		double GetHighestPrecedence(std::size_t from, std::size_t to, std::vector<StackElem>& stack);
		std::vector<std::size_t> GetAllPrecedenceIndex(std::size_t from, std::size_t to, double p, std::vector<StackElem>& stack);
		void ReduceInfixAt(std::size_t idx, std::vector<StackElem>& stack);
		void Reduce(std::size_t from, std::size_t to, std::vector<StackElem>& stack);

		Expr* ParseAtom();

		Expr* ParseFuncExpr();
		BlockExpr* ParseBlockExpr();

		TypeDesc* ParseType();
	};
}