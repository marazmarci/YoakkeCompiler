#include <sstream>
#include "Parser.h"
#include "ExprParser.h"
#include "../Utils/StringUtils.h"

namespace yk
{
	Parser::Parser()
		: m_CurrentToken(Token(TokenT::Epsilon, "", 0, 0)), m_Logger(Logger("Parser"))
	{
		m_Lexer.AddLexeme(";", TokenT::Keyword);
		m_Lexer.AddLexeme(":", TokenT::Keyword);
		m_Lexer.AddLexeme("(", TokenT::Keyword);
		m_Lexer.AddLexeme(")", TokenT::Keyword);
		m_Lexer.AddLexeme("{", TokenT::Keyword);
		m_Lexer.AddLexeme("}", TokenT::Keyword);
		m_Lexer.AddLexeme(",", TokenT::Keyword);
		m_Lexer.AddLexeme("->", TokenT::Keyword);

		AddInfixOp(BinOp("::", 0, AssocT::Noassoc));

		AddInfixOp(BinOp("+", 1, AssocT::Left));
		AddInfixOp(BinOp("-", 1, AssocT::Left));
		AddInfixOp(BinOp("*", 2, AssocT::Left));
		AddInfixOp(BinOp("/", 2, AssocT::Left));

		AddPrefixOp(UryOp("++", 3, FixityT::Prefix));
		AddPrefixOp(UryOp("--", 3, FixityT::Prefix));

		AddPostfixOp(UryOp("++", 3, FixityT::Postfix));
		AddPostfixOp(UryOp("--", 3, FixityT::Postfix));
	}

	void Parser::Next()
	{
		m_CurrentToken = m_Lexer.Next();
	}
	
	bool Parser::Same(std::string const& val)
	{
		return m_CurrentToken.Value == val;
	}

	bool Parser::Match(std::string const& val)
	{
		if (Same(val))
		{
			Next();
			return true;
		}
		return false;
	}

	bool Parser::Expect(std::string const& val)
	{
		if (!Match(val))
		{
			ExpectError('\'' + val + '\'', DumpCurrentTok());
			return false;
		}
		return true;
	}

	bool Parser::IsIdent()
	{
		return m_CurrentToken.Type == TokenT::Identifier;
	}

	std::string const& Parser::GetValue()
	{
		return m_CurrentToken.Value;
	}

	void Parser::ErrorAt(std::string const& msg, Token const& t)
	{
		std::size_t len = t.Value.size();
		std::size_t pos = t.Column - len;
		m_Logger.log() << "Syntax error "
			<< DumpPosition(t) << log::endl
			<< StringUtils::GetLine(m_Lexer.m_Buffer, t.Row)
			<< log::endl << StringUtils::GenArrow(pos, len) << log::endl
			<< msg << log::endlog;
	}

	void Parser::ExpectError(std::string const& ex, std::string const& fnd)
	{
		ExpectErrorAt(ex, fnd, m_CurrentToken);
	}

	void Parser::ExpectErrorAt(std::string const& ex, std::string const& fnd, Token const& t)
	{
		std::size_t len = t.Value.size();
		std::size_t pos = t.Column - len;
		if (t.Value == " <End of File> ")
		{
			m_Logger.log() << "Syntax error "
				<< DumpPosition(t) << log::endl
				<< ex << " expected, early end of file found."
				<< log::endlog;
		}
		else
		{
			m_Logger.log() << "Syntax error "
				<< DumpPosition(t) << log::endl
				<< StringUtils::GetLine(m_Lexer.m_Buffer, t.Row)
				<< log::endl << StringUtils::GenArrow(pos, len) << log::endl
				<< ex << " expected, found " << fnd
				<< '.' << log::endlog;
		}
	}

	std::string Parser::DumpPosition(Token const& t)
	{
		std::stringstream ss;
		ss << "at line " << t.Row << ", character " << t.Column;
		return ss.str();
	}

	std::string Parser::DumpCurrentTok()
	{
		return '\'' + m_CurrentToken.Value + '\'';
	}

	ParseState Parser::SaveState()
	{
		return ParseState(m_Lexer.m_Ptr, m_CurrentToken, m_Lexer.m_RowCount, m_Lexer.m_ColCount);
	}

	void Parser::LoadState(ParseState const& st)
	{
		m_Lexer.m_Ptr = st.SourcePointer;
		m_CurrentToken = st.CurrentToken;
		m_Lexer.m_ColCount = st.ColCount;
		m_Lexer.m_RowCount = st.RowCount;
	}

	void Parser::AddPrefixOp(UryOp op)
	{
		m_Lexer.AddLexeme(op.Symbol, TokenT::Operator);
		m_PrefixOps.insert(std::make_pair(op.Symbol, op));
	}

	void Parser::AddInfixOp(BinOp op)
	{
		m_Lexer.AddLexeme(op.Symbol, TokenT::Operator);
		m_InfixOps.insert(std::make_pair(op.Symbol, op));
	}

	void Parser::AddPostfixOp(UryOp op)
	{
		m_Lexer.AddLexeme(op.Symbol, TokenT::Operator);
		m_PostfixOps.insert(std::make_pair(op.Symbol, op));
	}

	UryOp* Parser::GetPrefixOp(std::string const& sym)
	{
		auto it = m_PrefixOps.find(sym);
		if (it == m_PrefixOps.end())
			return nullptr;
		return &it->second;
	}

	BinOp* Parser::GetInfixOp(std::string const& sym)
	{
		auto it = m_InfixOps.find(sym);
		if (it == m_InfixOps.end())
			return nullptr;
		return &it->second;
	}

	UryOp* Parser::GetPostfixOp(std::string const& sym)
	{
		auto it = m_PostfixOps.find(sym);
		if (it == m_PostfixOps.end())
			return nullptr;
		return &it->second;
	}

	std::vector<Stmt*> Parser::ParseProgram(const char* src)
	{
		m_Lexer.SetSource(src);
		Next();

		return ParseStmtList();
	}

	std::vector<Stmt*> Parser::ParseStmtList()
	{
		std::vector<Stmt*> list;
		while (true)
		{
			auto res = ParseStmt();
			if (res.size())
			{
				for (auto el : res)
					list.push_back(el);
			}
			else
				break;
		}
		return list;
	}

	std::vector<Stmt*> Parser::ParseStmt()
	{
		auto ls = ParseExprStmt();
		return ls;
	}

	std::vector<Stmt*> Parser::ParseExprStmt()
	{
		auto exps = ParseExprs();
		if (exps.size())
		{
			std::vector<Stmt*> ret;
			for (std::size_t i = 0; i < exps.size() - 1; i++)
			{
				auto el = exps[i];
				ret.push_back(new ExprStmt(el, false));
			}

			ret.push_back(new ExprStmt(exps[exps.size() - 1], Match(";")));
			return ret;
		}

		return std::vector<Stmt*>();
	}

	std::vector<Expr*> Parser::ParseExprs()
	{
		ExprParser expp(*this);
		return expp.ParseExprList();
	}

	Expr* Parser::ParseSingleExpr()
	{
		ExprParser expp(*this);
		auto ls = expp.ParseExprList();

		if (ls.size() == 0)
		{
			return nullptr;
		}
		if (ls.size() == 1)
		{
			return ls[0];
		}

		ExpectError("A single expression is", "multiple");
		return nullptr;
	}

	Expr* Parser::ParseAtom()
	{
		if (Expr* e = ParseFuncExpr())
		{
			return e;
		}
		else if (m_CurrentToken.Type == TokenT::Integer)
		{
			int v = std::atoi(GetValue().c_str());
			Next();
			return new IntLiteralExpr(v);
		}
		else if (Match("("))
		{
			Expr* exp = ParseSingleExpr();
			if (exp)
			{
				if (Expect(")"))
				{
					return exp;
				}
				else
				{
					return nullptr;
				}
			}
			else
			{
				ExpectError("Expression", DumpCurrentTok());
				return nullptr;
			}
		}
		else if (IsIdent())
		{
			std::string id = GetValue();
			Next();
			return new IdentExpr(id);
		}

		return nullptr;
	}

	Expr* Parser::ParseFuncExpr()
	{
		auto state = SaveState();
		FuncPrototype* proto = ParseFuncPrototype();
		if (proto)
		{
			if (BlockExpr* block = ParseBlockExpr())
			{
				return new FuncExpr(proto, block);
			}
			else
			{
				return new FuncHeaderExpr(proto);
			}
		}

		LoadState(state);
		return nullptr;
	}

	ParamPair Parser::ParseParameter()
	{
		auto state = SaveState();
		TypeDesc* type = nullptr;
		std::string name = "";

		if (IsIdent())
		{
			name = GetValue();
			Next();
		}

		if (Match(":"))
		{
			type = ParseType();
		}
		else if (name.length())
		{
			ExpectError("Type", DumpCurrentTok());
		}

		return std::make_pair(name, type);
	}

	FuncPrototype* Parser::ParseFuncPrototype()
	{
		auto state = SaveState();
		if (Match("("))
		{
			std::vector<ParamPair> params;
			auto par = ParseParameter();
			if (par.second)
			{
				params.push_back(par);
				while (Match(","))
				{
					par = ParseParameter();
					if (par.second)
					{
						params.push_back(par);
					}
					else
					{
						ExpectError("Parameter", DumpCurrentTok());
						return nullptr;
					}
				}
			}

			if (Expect(")"))
			{
				TypeDesc* rettype = new IdentTypeDesc("unit");
				if (Match("->"))
				{
					delete rettype;
					rettype = ParseType();
					if (!rettype)
					{
						ExpectError("Return type", DumpCurrentTok());
						return nullptr;
					}
				}

				return new FuncPrototype(params, rettype);
			}
			else
			{
				return nullptr;
			}
		}

		LoadState(state);
		return nullptr;
	}

	BlockExpr* Parser::ParseBlockExpr()
	{
		if (Match("{"))
		{
			auto stmts = ParseStmtList();
			if (Expect("}"))
			{
				return new BlockExpr(stmts);
			}
		}

		return nullptr;
	}

	TypeDesc* Parser::ParseType()
	{
		if (IsIdent())
		{
			std::string id = GetValue();
			Next();
			return new IdentTypeDesc(id);
		}

		return nullptr;
	}
}