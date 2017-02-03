#pragma once

#include <vector>
#include "Lexer.h"
#include "Operator.h"
#include "ParseState.h"
#include "../AST/Expr.h"
#include "../AST/Stmt.h"
#include "../AST/TypeDesc.h"
#include "../Logging/Logger.h"

namespace yk
{
	class Parser
	{
	private:
		friend class ExprParser;

	private:
		yk::Lexer m_Lexer;
		Token m_CurrentToken;
		ymap<ystr, BinOp> m_InfixOps;
		ymap<ystr, UryOp> m_PrefixOps;
		ymap<ystr, UryOp> m_PostfixOps;
		yvec<MixfixOp> m_MixfixOps;
		Logger m_Logger;

	public:
		Parser();

	private:
		void Next();
		bool Same(ystr const& val);
		bool Match(ystr const& val);
		bool Expect(ystr const& val);
		bool IsIdent();
		ystr const& GetValue();

		void ErrorAt(ystr const& msg, Token const& t);
		void ExpectError(ystr const& ex, ystr const& fnd);
		void ExpectErrorAt(ystr const& ex, ystr const& fnd, Token const& t);
		ystr DumpCurrentTok();

		ParseState SaveState();
		void LoadState(ParseState const& st);

		void AddPrefixOp(UryOp op);
		void AddInfixOp(BinOp op);
		void AddPostfixOp(UryOp op);
		void AddMixfixOp(MixfixOp op);

	public:
		UryOp* GetPrefixOp(ystr const& sym);
		BinOp* GetInfixOp(ystr const& sym);
		UryOp* GetPostfixOp(ystr const& sym);

		yvec<Stmt*> ParseProgram(const char* src);

	private:
		yvec<Stmt*> ParseStmtList();
		yvec<Stmt*> ParseStmt();
		yvec<Stmt*> ParseExprStmt();
		yvec<Expr*> ParseExprs();

		Expr* ParseSingleExpr();
		Expr* ParseAtom();

		ParamExpr*		ParseParameter();
		FuncHeaderExpr*	ParseFuncPrototype();
		BlockExpr*		ParseBlockExpr();
		Expr*			ParseFuncExpr();

		TypeDesc* ParseType();
	};
}