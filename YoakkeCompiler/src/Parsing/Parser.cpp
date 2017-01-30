#include "Parser.h"
#include "ExprParser.h"

namespace yk
{
	Parser::Parser()
		: m_CurrentToken(Token(TokenT::Epsilon, "", 0, 0))
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
	}

	void Parser::Error(std::string const& msg)
	{
		std::cout << "Syntax error: " << msg << std::endl;
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

	bool Parser::IsIdent()
	{
		return m_CurrentToken.Type == TokenT::Identifier;
	}

	std::string const& Parser::GetValue()
	{
		return m_CurrentToken.Value;
	}

	ParseState Parser::SaveState()
	{
		return ParseState(m_Lexer.m_Ptr, m_CurrentToken, m_Lexer.m_RowCount, m_Lexer.m_ColCount);
	}

	void Parser::LoadState(ParseState const& st)
	{
		m_Lexer.m_Ptr = st.SourcePointer;
		m_CurrentToken = st.CurrentToken;
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

		Error("Only a single expression is expected!");
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
				if (Match(")"))
				{
					return exp;
				}
				else
				{
					Error("')' expected!");
					return nullptr;
				}
			}
			else
			{
				Error("Expression expected after '('!");
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
			Error("Type expected after identifier!");
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
						Error("Parameter expected!");
						return nullptr;
					}
				}
			}

			if (Match(")"))
			{
				TypeDesc* rettype = new IdentTypeDesc("unit");
				if (Match("->"))
				{
					delete rettype;
					rettype = ParseType();
					if (!rettype)
					{
						Error("Return type expected after '->'!");
						return nullptr;
					}
				}

				return new FuncPrototype(params, rettype);
			}
			else
			{
				Error("')' exprected");
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
			if (Match("}"))
			{
				return new BlockExpr(stmts);
			}
			else
			{
				Error("'}' expected!");
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