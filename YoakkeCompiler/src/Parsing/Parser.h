#pragma once

#include <vector>
#include "Lexer.h"
#include "../AST/Expr.h"
#include "../AST/FuncPrototype.h"
#include "../AST/Stmt.h"
#include "../AST/TypeDesc.h"

namespace yk
{
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

	public:
		std::vector<Stmt*> ParseProgram(const char* src);

	private:
		Stmt* ParseStmt();
		Stmt* ParseConstDecl();

		ParamPair ParseParameter();
		FuncPrototype* ParseFuncPrototype();
		Expr* ParseExpr();
		Expr* ParseFuncHeaderExpr();
		Expr* ParseFuncExpr();
		BlockExpr* ParseBlockExpr();

		TypeDesc* ParseType();
	};
}