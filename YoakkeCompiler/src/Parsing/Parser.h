#pragma once

#include <vector>
#include "Lexer.h"
#include "Operator.h"
#include "ParseState.h"
#include "../AST/Expr.h"
#include "../AST/FuncPrototype.h"
#include "../AST/Stmt.h"
#include "../AST/TypeDesc.h"

namespace yk
{
	class Parser
	{
	private:
		friend class ExprParser;

	private:
		yk::Lexer m_Lexer;
		Token m_CurrentToken;
		std::map<std::string, BinOp> m_InfixOps;
		std::map<std::string, UryOp> m_PrefixOps;
		std::map<std::string, UryOp> m_PostfixOps;

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

		void AddPrefixOp(UryOp op);
		void AddInfixOp(BinOp op);
		void AddPostfixOp(UryOp op);

	public:
		UryOp* GetPrefixOp(std::string const& sym);
		BinOp* GetInfixOp(std::string const& sym);
		UryOp* GetPostfixOp(std::string const& sym);

		std::vector<Stmt*> ParseProgram(const char* src);

	private:
		std::vector<Stmt*> ParseStmtList();
		std::vector<Stmt*> ParseStmt();
		std::vector<Stmt*> ParseExprStmt();
		std::vector<Expr*> ParseExprs();

		Expr* ParseSingleExpr();
		Expr* ParseAtom();

		ParamPair ParseParameter();
		FuncPrototype* ParseFuncPrototype();
		BlockExpr* ParseBlockExpr();
		Expr* ParseFuncExpr();

		TypeDesc* ParseType();
	};
}