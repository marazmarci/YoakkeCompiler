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
		m_Lexer.AddLexeme("->", TokenT::Keyword);
		m_Lexer.AddLexeme("let", TokenT::Keyword);

		// Default operators ////////////////////////////////////////
		AddInfixOp(BinOp("::", 0, AssocT::Noassoc));

		AddInfixOp(BinOp(",", 1, AssocT::Left));

		AddInfixOp(BinOp("=", 2, AssocT::Right));
		
		AddInfixOp(BinOp("||", 3, AssocT::Left));
		
		AddInfixOp(BinOp("&&", 4, AssocT::Left));

		AddInfixOp(BinOp("==", 5, AssocT::Left));
		AddInfixOp(BinOp("<>", 5, AssocT::Left));

		AddInfixOp(BinOp(">", 6, AssocT::Left));
		AddInfixOp(BinOp("<", 6, AssocT::Left));
		AddInfixOp(BinOp(">=", 6, AssocT::Left));
		AddInfixOp(BinOp("<=", 6, AssocT::Left));

		AddInfixOp(BinOp("+", 7, AssocT::Left));
		AddInfixOp(BinOp("-", 7, AssocT::Left));

		AddInfixOp(BinOp("*", 8, AssocT::Left));
		AddInfixOp(BinOp("/", 8, AssocT::Left));
		AddInfixOp(BinOp("%", 8, AssocT::Left));

		AddPrefixOp(UryOp("+", 9, FixityT::Prefix));
		AddPrefixOp(UryOp("-", 9, FixityT::Prefix));
		AddPrefixOp(UryOp("!", 9, FixityT::Prefix));

		AddInfixOp(BinOp(".", 10, AssocT::Left));
		/////////////////////////////////////////////////////////////
	}

	void Parser::Next()
	{
		m_CurrentToken = m_Lexer.Next();
	}
	
	bool Parser::Same(ystr const& val)
	{
		return m_CurrentToken.Value == val;
	}

	bool Parser::Match(ystr const& val)
	{
		if (Same(val))
		{
			Next();
			return true;
		}
		return false;
	}

	bool Parser::Expect(ystr const& val)
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

	ystr const& Parser::GetValue()
	{
		return m_CurrentToken.Value;
	}

	void Parser::ErrorAt(ystr const& msg, Token const& t)
	{
		ysize len = t.Value.size();
		ysize pos = t.Column;
		ystr ln = StringUtils::GetLine(m_Lexer.m_Buffer, t.Row);
		m_Logger.log() << "Syntax error "
			<< StringUtils::Position(NodePos::Get(t)) << log::endl
			<< ln
			<< log::endl << StringUtils::GenArrow(pos, len, ln) << log::endl
			<< msg << log::endlog;
	}

	void Parser::ExpectError(ystr const& ex, ystr const& fnd)
	{
		ExpectErrorAt(ex, fnd, m_CurrentToken);
	}

	void Parser::ExpectErrorAt(ystr const& ex, ystr const& fnd, Token const& t)
	{
		ysize len = t.Value.size();
		ysize pos = t.Column - len;
		if (t.Value == " <End of File> ")
		{
			m_Logger.log() << "Syntax error "
				<< StringUtils::Position(NodePos::Get(t)) << log::endl
				<< ex << " expected, early end of file found."
				<< log::endlog;
		}
		else
		{
			ystr ln = StringUtils::GetLine(m_Lexer.m_Buffer, t.Row);
			m_Logger.log() << "Syntax error "
				<< StringUtils::Position(NodePos::Get(t)) << log::endl
				<< ln
				<< log::endl << StringUtils::GenArrow(pos, len, ln) << log::endl
				<< ex << " expected, found " << fnd
				<< '.' << log::endlog;
		}
	}

	ystr Parser::DumpCurrentTok()
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

	void Parser::AddMixfixOp(MixfixOp op)
	{
		m_MixfixOps.push_back(op);
	}

	UryOp* Parser::GetPrefixOp(ystr const& sym)
	{
		auto it = m_PrefixOps.find(sym);
		if (it == m_PrefixOps.end())
			return nullptr;
		return &it->second;
	}

	BinOp* Parser::GetInfixOp(ystr const& sym)
	{
		auto it = m_InfixOps.find(sym);
		if (it == m_InfixOps.end())
			return nullptr;
		return &it->second;
	}

	UryOp* Parser::GetPostfixOp(ystr const& sym)
	{
		auto it = m_PostfixOps.find(sym);
		if (it == m_PostfixOps.end())
			return nullptr;
		return &it->second;
	}

	yvec<Stmt*> Parser::ParseProgram(const char* src)
	{
		m_Lexer.SetSource(src);
		Next();

		return ParseStmtList();
	}

	yvec<Stmt*> Parser::ParseStmtList()
	{
		yvec<Stmt*> list;
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

	yvec<Stmt*> Parser::ParseStmt()
	{
		auto ls = ParseExprStmt();
		return ls;
	}

	yvec<Stmt*> Parser::ParseExprStmt()
	{
		auto exps = ParseExprs();
		if (exps.size())
		{
			yvec<Stmt*> ret;
			for (ysize i = 0; i < exps.size() - 1; i++)
			{
				auto el = exps[i];
				ret.push_back(new ExprStmt(el, false));
			}

			Token curr = m_CurrentToken;
			ret.push_back(new ExprStmt(exps[exps.size() - 1], 
				Match(";") ? (&curr) : (nullptr)));
			return ret;
		}

		return yvec<Stmt*>();
	}

	yvec<Expr*> Parser::ParseExprs()
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
			Token t = m_CurrentToken;
			int v = std::atoi(GetValue().c_str());
			Next();
			return new IntLiteralExpr(t);
		}
		else if (Same("("))
		{
			Token beg = m_CurrentToken;
			Next();
			Expr* exp = ParseSingleExpr();
			if (exp)
			{
				if (Same(")"))
				{
					Token end = m_CurrentToken;
					Next();
					return exp;
				}
				else
				{
					ExpectError("')'", DumpCurrentTok());
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
			ystr id = GetValue();
			Token curr = m_CurrentToken;
			Next();
			return new IdentExpr(curr);
		}
		else if (Same("let"))
		{
			NodePos curr = NodePos::Get(m_CurrentToken);
			Next();
			Expr* lval = ParseSingleExpr();
			if (lval)
			{
				NodePos last = lval->Position;
				TypeDesc* type = nullptr;
				if (Match(":"))
				{
					type = ParseType();
					if (!type)
					{
						ExpectError("Type", DumpCurrentTok());
					}
					else
					{
						last = type->Position;
					}
				}
				Expr* rval = nullptr;
				if (Match("="))
				{
					rval = ParseSingleExpr();
					if (!rval)
					{
						ExpectError("Expression", DumpCurrentTok());
					}
					else
					{
						last = rval->Position;
					}
				}
				return new LetExpr(lval, type, rval,
					NodePos::Interval(curr, last));
			}
			else
			{
				ExpectError("Lvalue", DumpCurrentTok());
			}
		}

		return nullptr;
	}

	Expr* Parser::ParseFuncExpr()
	{
		auto state = SaveState();
		FuncHeaderExpr* proto = ParseFuncPrototype();
		if (proto)
		{
			if (BlockExpr* block = ParseBlockExpr())
			{
				return new FuncExpr(proto, block);
			}
			else
			{
				return proto;
			}
		}

		LoadState(state);
		return nullptr;
	}

	ParamExpr* Parser::ParseParameter()
	{
		auto state = SaveState();
		TypeDesc* type = nullptr;
		ystr name = "";
		Token ident = Token(TokenT::Identifier, "", m_CurrentToken.Row, m_CurrentToken.Column);
		bool gotid = false;

		if (IsIdent())
		{
			ident = m_CurrentToken;
			name = GetValue();
			Next();
			gotid = true;
		}

		if (Match(":"))
		{
			type = ParseType();
			if (type)
			{
				return new ParamExpr(gotid ? &ident : nullptr, type);
			}
			else
			{
				ExpectError("Type", DumpCurrentTok());
			}
		}
		else if (gotid)
		{
			ExpectError("Type", DumpCurrentTok());
		}

		return nullptr;
	}

	FuncHeaderExpr* Parser::ParseFuncPrototype()
	{
		auto state = SaveState();
		if (Match("("))
		{
			yvec<ParamExpr*> params;
			auto par = ParseParameter();
			if (par)
			{
				params.push_back(par);
				while (Match(","))
				{
					par = ParseParameter();
					if (par)
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
				TypeDesc* rettype = new IdentTypeDesc(Token(TokenT::Keyword, "unit", 0, 0));
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

				return new FuncHeaderExpr(params, rettype, 
					NodePos(state.ColCount, state.RowCount, 
						rettype->Position.EndX, rettype->Position.EndY));
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
		if (Same("{"))
		{
			Token beg = m_CurrentToken;
			Next();
			auto stmts = ParseStmtList();
			if (Same("}"))
			{
				Token end = m_CurrentToken;
				Next();
				return new BlockExpr(stmts, NodePos::Interval(beg, end));
			}
			else
			{
				ExpectError("'}'", DumpCurrentTok());
			}
		}

		return nullptr;
	}

	TypeDesc* Parser::ParseType()
	{
		if (IsIdent())
		{
			ystr id = GetValue();
			Token curr = m_CurrentToken;
			Next();
			return new IdentTypeDesc(curr);
		}

		return nullptr;
	}
}