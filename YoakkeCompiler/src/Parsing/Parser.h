#pragma once

#include <vector>
#include "Lexer.h"
#include "OperDesc.h"
#include "../AST/Expr.h"
#include "../AST/FuncPrototype.h"
#include "../AST/Stmt.h"
#include "../AST/TypeDesc.h"

namespace yk
{
	enum class StackElemT
	{
		Expr, Oper
	};

	class StackElem
	{
	public:
		StackElemT Tag;
		Token Oper;
		Expr* Exp;

	public:
		StackElem(Token o)
			: Tag(StackElemT::Oper), Oper(o), Exp(nullptr)
		{
		}

		StackElem(Expr* e)
			: Tag(StackElemT::Expr), Exp(e), Oper(Token::EPSILON)
		{
		}
	};

	class ParseState
	{
	public:
		const char* SourcePointer;
		Token CurrentToken;

	public:
		ParseState(const char* src, Token ct)
			: SourcePointer(src), CurrentToken(ct)
		{
		}
	};

	class Parser
	{
	private:
		yk::Lexer m_Lexer;
		Token m_CurrentToken;
		std::map<std::string, OperDesc> m_Operators;

	public:
		Parser();

	private:
		void Next();
		bool Same(std::string const& val);
		bool Match(std::string const& val);
		bool IsIdent();
		std::string const& GetValue();
		
		ParseState SaveState();
		void LoadState(ParseState const& st);

		void AddOperator(std::string sym, OperDesc desc);
		OperDesc* GetOperator(Token t);

	public:
		std::vector<Stmt*> ParseProgram(const char* src);

	private:
		Stmt* ParseStmt();
		Stmt* ParseConstDecl();

		ParamPair ParseParameter();
		FuncPrototype* ParseFuncPrototype();

		Expr* ParseExpr();

		Expr* ParsePrecedence();
		double GetHighestPrecedence(std::size_t from, std::size_t to, std::vector<StackElem>& stack);
		std::vector<std::size_t> GetAllPrecedenceIndex(std::size_t from, std::size_t to, double p, std::vector<StackElem>& stack);
		void ReduceInfixAt(std::size_t idx, std::vector<StackElem>& stack);
		void Reduce(std::size_t from, std::size_t to, std::vector<StackElem>& stack);

		Expr* ParseAtom();

		Expr* ParseFuncHeaderExpr();
		Expr* ParseFuncExpr();
		BlockExpr* ParseBlockExpr();

		TypeDesc* ParseType();
	};
}