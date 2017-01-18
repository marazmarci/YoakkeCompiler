#include "Parser.h"

namespace yk
{
	Parser::Parser()
		: m_CurrentToken(Token::EPSILON)
	{
		m_Lexer.AddLexeme(";", TokenT::Keyword);
		m_Lexer.AddLexeme(":", TokenT::Keyword);
		m_Lexer.AddLexeme("::", TokenT::Keyword);
		m_Lexer.AddLexeme("(", TokenT::Keyword);
		m_Lexer.AddLexeme(")", TokenT::Keyword);
		m_Lexer.AddLexeme("{", TokenT::Keyword);
		m_Lexer.AddLexeme("}", TokenT::Keyword);
		m_Lexer.AddLexeme(",", TokenT::Keyword);
		m_Lexer.AddLexeme("->", TokenT::Keyword);
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
		return ParseState(m_Lexer.m_Ptr, m_CurrentToken);
	}

	void Parser::LoadState(ParseState const& st)
	{
		m_Lexer.m_Ptr = st.SourcePointer;
		m_CurrentToken = st.CurrentToken;
	}

	std::vector<Stmt*> Parser::ParseProgram(const char* src)
	{
		m_Lexer.SetSource(src);
		Next();

		std::vector<Stmt*> prog;
		Stmt* st = nullptr;
		while (st = ParseConstDecl()) prog.push_back(st);

		return prog;
	}

	Stmt* Parser::ParseStmt()
	{
		Stmt* st = nullptr;
		if (st = ParseConstDecl()) return st;

		return nullptr;
	}

	Stmt* Parser::ParseConstDecl()
	{
		auto state = SaveState();
		if (IsIdent())
		{
			std::string id = GetValue();
			Next();
			if (Match("::"))
			{
				if (Expr* exp = ParseExpr())
				{
					return new ConstDeclStmt(id, exp);
				}
				else
				{
					std::cout << "Expression expected!" << std::endl;
					return nullptr;
				}
			}
		}

		LoadState(state);
		return nullptr;
	}

	Expr* Parser::ParseExpr()
	{
		Expr* exp = nullptr;
		if (exp = ParseFuncExpr()) return exp;

		return nullptr;
	}

	ParamPair Parser::ParseParameter()
	{
		auto state = SaveState();
		TypeDesc* type = nullptr;

		if (IsIdent())
		{
			std::string name = GetValue();
			Next();
			if (Match(":"))
			{
				if (type = ParseType())
				{
					return std::make_pair(name, type);
				}
				else
				{
					std::cout << "Type expected!" << std::endl;
					return std::make_pair("", nullptr);
				}
			}
		}
		LoadState(state);
		type = ParseType();

		return std::make_pair("", type);
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
						std::cout << "Parameter expected!" << std::endl;
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
						std::cout << "Return type expected after '->'!" << std::endl;
						return nullptr;
					}
				}

				return new FuncPrototype(params, rettype);
			}
			else
			{
				std::cout << "')' exprected" << std::endl;
				return nullptr;
			}
		}

		LoadState(state);
		return nullptr;
	}

	Expr* Parser::ParseFuncHeaderExpr()
	{
		auto state = SaveState();
		FuncPrototype* proto = ParseFuncPrototype();
		if (proto)
		{
			if (Match(";"))
			{
				return new FuncHeaderExpr(proto);
			}
		}

		LoadState(state);
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
			if (Match(";"))
			{
				return new FuncHeaderExpr(proto);
			}

			std::cout << "Expected block or ';'!" << std::endl;
			return nullptr;
		}

		LoadState(state);
		return nullptr;
	}

	BlockExpr* Parser::ParseBlockExpr()
	{
		if (Match("{"))
		{
			std::vector<Stmt*> stmts;
			Stmt* st = nullptr;
			while (st = ParseStmt()) stmts.push_back(st);
			if (Match("}"))
			{
				return new BlockExpr(stmts);
			}
			else
			{
				std::cout << "'}' expected!" << std::endl;
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