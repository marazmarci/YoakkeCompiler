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

		AddInfixOp(BinOp("+", 4, AssocT::Left));
		AddInfixOp(BinOp("-", 4, AssocT::Left));
		AddInfixOp(BinOp("*", 5, AssocT::Left));
		AddInfixOp(BinOp("/", 5, AssocT::Left));
		AddPrefixOp(UryOp("+", 6, FixityT::Prefix));
		AddPrefixOp(UryOp("-", 6, FixityT::Prefix));
		AddPrefixOp(UryOp("++", 7, FixityT::Prefix));
		AddPrefixOp(UryOp("--", 7, FixityT::Prefix));
		AddPostfixOp(UryOp("++", 7, FixityT::Postfix));
		AddPostfixOp(UryOp("--", 7, FixityT::Postfix));
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

		ParseExpr();
		for (;;);

		std::vector<Stmt*> prog;
		Stmt* st = nullptr;
		while (st = ParseStmt()) prog.push_back(st);

		return prog;
	}

	Stmt* Parser::ParseStmt()
	{
		Stmt* st = nullptr;
		if (st = ParseExprStmt()) return st;

		return nullptr;
	}

	Stmt* Parser::ParseExprStmt()
	{
		auto state = SaveState();
		Expr* exp = ParseExpr();
		bool need_semicol = true;
		if (exp)
		{
			if (BinExpr* be = dynamic_cast<BinExpr*>(exp))
			{
				if (dynamic_cast<FuncExpr*>(be->RHS))
				{
					need_semicol = false;
				}
			}

			if (need_semicol)
			{
				if (!Match(";"))
				{
					Error("';' expected!");
					return nullptr;
				}
			}

			return new ExprStmt(exp, Match(";"));
		}

		LoadState(state);
		return nullptr;
	}

	Expr* Parser::ParseExpr()
	{
		ExprParser expp(*this);
		expp.ParseExprList();
		/*
		std::vector<StackElem> stack;

		// Parse atomic expressions and operators onto the stack /////
		OperDesc* op = nullptr;
		Expr* exp = nullptr;
		while (true)
		{
			if (op = GetBinOp(m_CurrentToken))
			{
				Token t = m_CurrentToken;
				Next();
				stack.push_back(StackElem(op, SaveState()));
			}
			else if (op = GetUryOp(m_CurrentToken))
			{
				Token t = m_CurrentToken;
				Next();
				stack.push_back(StackElem(op, SaveState()));
			}
			else if (exp = ParseAtom())
			{
				stack.push_back(StackElem(exp, SaveState()));
			}
			else
			{
				// Add a top save
				stack.push_back(StackElem(SaveState()));
				break;
			}
		}
		//////////////////////////////////////////////////////////////

		// Reduce until no further reduce is possible ////////////////
		while (true)
		{
			auto prevsz = stack.size();
			Reduce(0, stack.size(), stack);
			if (stack.size() == prevsz)
			{
				if (stack.size() == 0)
					return nullptr;

				if (stack.size() > 1)
					LoadState(stack[1].State);

				if (stack[0].Tag == StackElemT::Expr)
					return stack[0].Get<Expr>();

				LoadState(stack[0].State);
				return nullptr;
			}
		}
		//////////////////////////////////////////////////////////////*/
		return nullptr;
	}

	/*
	double Parser::GetHighestPrecedence(std::size_t from, std::size_t to, std::vector<StackElem>& stack)
	{
		double max = -1;
		for (std::size_t i = from; i < to; i++)
		{
			auto& el = stack[i];
			if (el.Tag == StackElemT::Oper && el.Get<OperDesc>()->Precedence > max)
			{
				max = el.Get<OperDesc>()->Precedence;
			}
		}
		return max;
	}

	std::vector<std::size_t> Parser::GetAllPrecedenceIndex(std::size_t from, std::size_t to, double p, std::vector<StackElem>& stack)
	{
		std::vector<std::size_t> idxes;
		for (std::size_t i = from; i < to; i++)
		{
			StackElem& e = stack[i];
			if (e.Tag == StackElemT::Oper && e.Get<OperDesc>()->Precedence == p)
			{
				idxes.push_back(i);
			}
		}
		return idxes;
	}

	bool Parser::NoassocSanity(std::vector<std::size_t> const& idxes, std::vector<StackElem>& stack)
	{
		std::vector<OperDesc*> noassoc;
		for (auto i : idxes)
		{
			StackElem& el = stack[i];
			OperDesc* op = el.Get<OperDesc>();
			if (op->Assoc == AssocT::Noassoc)
				noassoc.push_back(op);
		}

		for (std::size_t i = 0; i < noassoc.size(); i++)
		{
			for (std::size_t j = i + 1; j < noassoc.size(); j++)
			{
				if (noassoc[i]->Symbol == noassoc[j]->Symbol)
					return false;
			}
		}

		return true;
	}

	void Parser::ReduceInfixAt(std::size_t idx, std::vector<StackElem>& stack)
	{
		if (stack[idx - 1].Tag == StackElemT::Expr &&
			stack[idx + 1].Tag == StackElemT::Expr)
		{
			ParseState save = stack[idx - 1].State;

			Expr* lhs = stack[idx - 1].Get<Expr>();
			Expr* rhs = stack[idx + 1].Get<Expr>();
			Expr* nexp = new BinExpr(lhs, rhs, stack[idx].Get<OperDesc>());

			// Delete LHS, operator and RHS
			stack.erase(stack.begin() + (idx - 1));
			stack.erase(stack.begin() + (idx - 1));
			stack.erase(stack.begin() + (idx - 1));
			// Insert new element
			if (idx - 1 == stack.size())
				stack.push_back(StackElem(nexp, save));
			else
				stack.insert(stack.begin() + (idx - 1), StackElem(nexp, save));
		}
		else
		{
			Error("Not operands around infix op!");
			return;
		}
	}

	void Parser::Reduce(std::size_t from, std::size_t to, std::vector<StackElem>& stack)
	{
		double maxprec = GetHighestPrecedence(from, to, stack);
		if (maxprec >= 0)
		{
			auto list = GetAllPrecedenceIndex(from, to, maxprec, stack);

			if (!NoassocSanity(list, stack))
			{
				Error("Cannot chain non-associative operators of the same kind!");
				return;
			}

			// Reduce noassoc
			for (std::size_t i = 0; i < list.size(); i++)
			{
				std::size_t idx = list[i];
				OperDesc* op = stack[idx].Get<OperDesc>();
				if (op->Fixity == FixityT::Infix && op->Assoc == AssocT::Noassoc)
				{
					if (idx == 0 || idx == stack.size() - 1)
					{
						Error("Infix at end or beginning!");
						return;
					}

					ReduceInfixAt(idx, stack);
					//debug_stack(m_Stack);
					list.erase(list.begin() + i);
					for (std::size_t j = i; j < list.size(); j++)
					{
						list[j] -= 2;
					}
					i = -1;
					continue;
				}
			}

			// Reduce INFIX LEFT
			for (std::size_t i = 0; i < list.size(); i++)
			{
				std::size_t idx = list[i];
				OperDesc* op = stack[idx].Get<OperDesc>();
				if (op->Fixity == FixityT::Infix && op->Assoc == AssocT::Left)
				{
					if (idx == 0 || idx == stack.size() - 1)
					{
						Error("Infix at end or beginning!");
						return;
					}

					ReduceInfixAt(idx, stack);
					//debug_stack(m_Stack);
					list.erase(list.begin() + i);
					for (std::size_t j = i; j < list.size(); j++)
					{
						list[j] -= 2;
					}
					i = -1;
					continue;
				}
			}

			// Reduce INFIX RIGHT
			for (int i = list.size() - 1; i >= 0; i--)
			{
				std::size_t idx = list[i];
				OperDesc* op = stack[idx].Get<OperDesc>();
				if (op->Fixity == FixityT::Infix && op->Assoc == AssocT::Right)
				{
					if (idx == 0 || idx == stack.size() - 1)
					{
						Error("Infix at end or beginning!");
						return;
					}

					ReduceInfixAt(idx, stack);
					//debug_stack(m_Stack);
					list.erase(list.begin() + i);
					for (std::size_t j = i; j < list.size(); j++)
					{
						list[j] -= 2;
					}
					i = list.size();
					continue;
				}
			}

			// Reduce POSTFIX
			for (std::size_t i = 0; i < list.size(); i++)
			{
				std::size_t idx = list[i];
				OperDesc* op = stack[idx].Get<OperDesc>();
				if (op->Fixity == FixityT::Postfix)
				{
					if (idx == 0)
					{
						Error("Postfix at beginning!");
						return;
					}

					// Find the first 2 expressions near each other without operator between
					int j = 0;
					bool found = false;
					for (j = idx - 1; j > 0; j--)
					{
						if (stack[j].Tag == StackElemT::Expr &&
							stack[j - 1].Tag == StackElemT::Expr)
						{
							found = true;
							break;
						}
					}
					if (!found)
					{
						Error("No operands for postfix operator!");
						return;
					}

					OperDesc* op2 = nullptr;
					for (j = j - 1; j >= 0; j -= 2)
					{
						if ((j > 0) &&
							(stack[j].Tag == StackElemT::Oper &&
								stack[j - 1].Tag == StackElemT::Expr))
						{
							op2 = stack[j].Get<OperDesc>();
							if (op2->Precedence >= op->Precedence)
								break;
						}
						else
						{
							break;
						}
					}

					auto prevsz = stack.size();
					Reduce(j, idx - 1, stack);
					//debug_stack(stack);
					if (stack.size() == prevsz)
					{
						ParseState save = stack[idx - 2].State;

						Expr* lhs = stack[idx - 2].Get<Expr>();
						Expr* rhs = stack[idx - 1].Get<Expr>();
						Expr* nexp = new BinExpr(lhs, rhs, stack[idx].Get<OperDesc>());

						// Delete LHS, RHS and operator
						stack.erase(stack.begin() + (idx - 2));
						stack.erase(stack.begin() + (idx - 2));
						stack.erase(stack.begin() + (idx - 2));

						// Insert new
						if (idx - 2 == stack.size())
							stack.push_back(StackElem(nexp, save));
						else
							stack.insert(stack.begin() + (idx - 2), StackElem(nexp, save));
					}
					return;
				}
			}

			// Reduce PREFIX
			for (int i = list.size() - 1; i >= 0; i--)
			{
				std::size_t idx = list[i];
				OperDesc* op = stack[idx].Get<OperDesc>();
				if (op->Fixity == FixityT::Prefix)
				{
					if (idx == stack.size() - 1)
					{
						Error("Prefix at end!");
						return;
					}

					// Find the first 2 expressions near each other without operator between
					int j = 0;
					bool found = false;
					for (j = idx + 1; j < stack.size() - 1; j++)
					{
						if (stack[j].Tag == StackElemT::Expr &&
							stack[j + 1].Tag == StackElemT::Expr)
						{
							found = true;
							break;
						}
					}
					if (!found)
					{
						Error("No operands for prefix operator!");
						return;
					}

					OperDesc* op2 = nullptr;
					for (j = j + 1; j < stack.size(); j += 2)
					{
						if ((j < stack.size() - 1) &&
							(stack[j].Tag == StackElemT::Oper &&
								stack[j + 1].Tag == StackElemT::Expr))
						{
							op2 = stack[j].Get<OperDesc>();
							if (op2->Precedence >= op->Precedence)
								break;
						}
						else
						{
							break;
						}
					}

					auto prevsz = stack.size();
					Reduce(idx + 1, j + 1, stack);
					//debug_stack(stack);
					if (stack.size() == prevsz)
					{
						ParseState save = stack[idx + 1].State;

						Expr* lhs = stack[idx + 1].Get<Expr>();
						Expr* rhs = stack[idx + 2].Get<Expr>();
						Expr* nexp = new BinExpr(lhs, rhs, stack[idx].Get<OperDesc>());

						// Delete LHS, RHS and operator
						stack.erase(stack.begin() + (idx));
						stack.erase(stack.begin() + (idx));
						stack.erase(stack.begin() + (idx));

						// Insert new
						if (idx == stack.size())
							stack.push_back(StackElem(nexp, save));
						else
							stack.insert(stack.begin() + idx, StackElem(nexp, save));
					}
					return;
				}
			}
		}
	}*/

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
			Expr* exp = ParseExpr();
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
			std::vector<Stmt*> stmts;
			Stmt* st = nullptr;
			while (st = ParseStmt()) stmts.push_back(st);
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