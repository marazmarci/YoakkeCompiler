#include "Parser.h"

namespace yk
{
	void Parser::Next()
	{
		if (m_State.m_Index >= m_Tokens.size())
		{
			m_State.m_Token = Token(TokenT::Epsilon, "");
		}
		else
		{
			m_State.m_Token = m_Tokens[m_State.m_Index++];
		}
	}

	ParseState Parser::SaveState()
	{
		return m_State;
	}

	void Parser::LoadState(ParseState st)
	{
		m_State = st;
	}

	bool Parser::Same(std::string const& tok)
	{
		return (m_State.m_Token.Type == TokenT::Symbol && m_State.m_Token.Value == tok);
	}

	bool Parser::Match(std::string const& tok)
	{
		if (Same(tok))
		{
			Next();
			return true;
		}
		return false;
	}

	std::string Parser::Value()
	{
		return m_State.m_Token.Value;
	}

	Token Parser::Peek()
	{
		return m_State.m_Token;
	}

	bool Parser::IsIdent()
	{
		return m_State.m_Token.Type == TokenT::Ident;
	}

	bool Parser::IsInteger()
	{
		return m_State.m_Token.Type == TokenT::Int;
	}

	bool Parser::IsFloat()
	{
		return m_State.m_Token.Type == TokenT::Float;
	}

	Operator* Parser::GetOp(operator_map& map, FixityT fix, OperatorT opt, Token t)
	{
		auto it = map.find(t.Value);
		if (it == map.end() || it->second.Fixity != fix || it->second.Type != opt)
		{
			return nullptr;
		}
		return &it->second;
	}

	Operator* Parser::PrefixBinaryOp(Token t)
	{
		return GetOp(m_PrefixOps, FixityT::Prefix, OperatorT::Binary, t);
	}

	Operator* Parser::InfixBinaryOp(Token t)
	{
		return GetOp(m_InfixOps, FixityT::Infix, OperatorT::Binary, t);
	}

	Operator* Parser::PostfixBinaryOp(Token t)
	{
		return GetOp(m_PostfixOps, FixityT::Postfix, OperatorT::Binary, t);
	}

	void Parser::AddPrefixBinaryOp(std::string const& sym, AssocT as, std::uint32_t prec)
	{
		if (m_Lexer.AddSymbol(sym))
		{
			m_PrefixOps.insert(std::make_pair(sym, Operator(OperatorT::Binary, FixityT::Prefix, sym, prec, as)));
		}
		else
		{
			std::cout << "Unable to define infix binary operator!" << std::endl;
		}
	}

	void Parser::AddInfixBinaryOp(std::string const& sym, AssocT as, std::uint32_t prec)
	{
		if (m_Lexer.AddSymbol(sym))
		{
			m_InfixOps.insert(std::make_pair(sym, Operator(OperatorT::Binary, FixityT::Infix, sym, prec, as)));
		}
		else
		{
			std::cout << "Unable to define infix binary operator!" << std::endl;
		}
	}

	void Parser::AddPostfixBinaryOp(std::string const& sym, AssocT as, std::uint32_t prec)
	{
		if (m_Lexer.AddSymbol(sym))
		{
			m_PostfixOps.insert(std::make_pair(sym, Operator(OperatorT::Binary, FixityT::Postfix, sym, prec, as)));
		}
		else
		{
			std::cout << "Unable to define infix binary operator!" << std::endl;
		}
	}

	void Parser::Init()
	{
		m_Lexer.AddSymbol("(");
		m_Lexer.AddSymbol(")");

		AddInfixBinaryOp(":=", AssocT::Right, 1);
		AddInfixBinaryOp("+", AssocT::Left, 2);
		AddPrefixBinaryOp("pre+", AssocT::Left, 2);
		AddPostfixBinaryOp("post+", AssocT::Left, 2);
		AddInfixBinaryOp("-", AssocT::Left, 2);
		AddPrefixBinaryOp("pre-", AssocT::Left, 2);
		AddPostfixBinaryOp("post-", AssocT::Left, 2);
		AddInfixBinaryOp("*", AssocT::Left, 3);
		AddPrefixBinaryOp("pre*", AssocT::Left, 3);
		AddPostfixBinaryOp("post*", AssocT::Left, 3);
		AddInfixBinaryOp("/", AssocT::Left, 3);
		AddPrefixBinaryOp("pre/", AssocT::Left, 3);
		AddPostfixBinaryOp("post/", AssocT::Left, 3);
	}

	Parser::Parser()
		: m_State(ParseState(0, Token(TokenT::Epsilon, ""))), m_Lexer(Lexer())
	{
		Init();
	}

	node_list Parser::Parse(std::string const& src)
	{
		m_Tokens = m_Lexer.Tokenize(src.c_str());
		Next();
		node_list ls;

		ls.push_back(ParseExpr());

		return ls;
	}

	Expr* Parser::ParseExpr()
	{
		return ParseSubExpr();
	}

	Expr* Parser::ParseSubExpr()
	{
		return ParseSubExpr1(nullptr, 0);
	}

	Expr* Parser::ParseSubExpr1(Expr* lhs, std::uint32_t min_prec)
	{
		Expr* rhs;
		Operator* op;
		Operator* op2;
		Token lookahead = Peek();
		if ((op = PrefixBinaryOp(lookahead)))
		{
			Next();
			if (lhs == nullptr)
			{
				lhs = ParseSubExpr1(nullptr, op->Precedence);
			}
			if (!lhs)
			{
				std::cout << "Error (code: 0)" << std::endl;
				return nullptr;
			}
			rhs = ParseSubExpr1(nullptr, op->Precedence);
			if (!rhs)
			{
				std::cout << "Error (code: 1)" << std::endl;
				return nullptr;
			}
			lhs = new BinExpr(lhs, rhs, op);
		}
		if (lhs == nullptr)
		{
			lhs = ParseAtom();
			if (!lhs) return nullptr;
		}
		lookahead = Peek();
		while (true)
		{
			lookahead = Peek();
			if (lhs != nullptr)
			{
				auto state = SaveState();
				rhs = ParseSubExpr1(nullptr, min_prec);
				if (rhs != nullptr)
				{
					lookahead = Peek();
					if ((op = PostfixBinaryOp(lookahead))/* && op->Precedence >= min_prec*/ && lhs != nullptr)
					{
						Next();
						lhs = new BinExpr(lhs, rhs, op);
						continue;
					}
					else
					{
						LoadState(state);
						//break;
					}
				}
				else
				{
					LoadState(state);
				}
			}
			
			if ((op = InfixBinaryOp(lookahead)) && op->Precedence >= min_prec)
			{
				Next();
				//rhs = ParseAtom();
				rhs = ParseSubExpr1(nullptr, op->Precedence + 1);
				if (rhs == nullptr)
				{
					std::cout << "Error (code: 2)" << std::endl;
					return nullptr;
				}
				lookahead = Peek();
				while ((op2 = InfixBinaryOp(lookahead)) &&
						((op2->Precedence > op->Precedence)
						|| (op2->Associativity == AssocT::Right && op2->Precedence == op->Precedence)))
				{
					rhs = ParseSubExpr1(rhs, op2->Precedence);
					lookahead = Peek();
				}
				lhs = new BinExpr(lhs, rhs, op);
				continue;
			}
			
			break;
		}

		return lhs;
	}
	
	Expr* Parser::ParseAtom()
	{
		if (IsInteger())
		{
			int v = std::atoi(Value().c_str());
			Next();
			return new IntConstExpr(v);
		}
		if (IsIdent())
		{
			std::string id = Value();
			Next();
			return new VarExpr(id);
		}
		if (Match("("))
		{
			Expr* exp = ParseExpr();
			if (Match(")"))
			{
				return exp;
			}
			std::cout << "')' is missing!" << std::endl;
		}

		return nullptr;
	}
}