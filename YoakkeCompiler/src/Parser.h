#pragma once

#include <map>
#include <vector>
#include "Lexer.h"
#include "AST.h"
#include "Expr.h"
#include "Token.h"
#include "Operator.h"

namespace yk
{
	typedef std::vector<ASTNode*> node_list;
	typedef std::map<std::string, Operator> operator_map;

	struct ParseState
	{
	public:
		std::size_t m_Index;
		Token m_Token;

	public:
		ParseState(std::size_t idx, Token t)
			: m_Index(idx), m_Token(t)
		{
		}
	};

	class Parser
	{
	private:
		token_list m_Tokens;
		ParseState m_State;
		Lexer m_Lexer;
		operator_map m_PrefixOps;
		operator_map m_InfixOps;
		operator_map m_PostfixOps;

	private:
		void Next();
		ParseState SaveState();
		void LoadState(ParseState st);

		bool Same(std::string const& tok);
		bool Match(std::string const& tok);
		std::string Value();
		Token Peek();

		bool IsIdent();
		bool IsInteger();
		bool IsFloat();
	
		Operator* GetOp(operator_map& map, FixityT fix, OperatorT opt, Token t);

		Operator* PrefixBinaryOp(Token t);
		Operator* InfixBinaryOp(Token t);
		Operator* PostfixBinaryOp(Token t);

		void AddPrefixBinaryOp(std::string const& sym, AssocT as, std::uint32_t prec);
		void AddInfixBinaryOp(std::string const& sym, AssocT as, std::uint32_t prec);
		void AddPostfixBinaryOp(std::string const& sym, AssocT as, std::uint32_t prec);

	public:
		Parser();
		void Init();
		node_list Parse(std::string const& src);
		Expr* ParseExpr();

	private:
		Expr* ParseSubExpr();
		Expr* ParseSubExpr1(Expr* lhs, std::uint32_t prec);
		Expr* ParseAtom();
	};
}