#include "Parser.h"

namespace yk
{
	Parser::Parser()
		: m_CurrentToken(Token::EPSILON)
	{
		m_Lexer.AddLexeme(";", TokenT::Keyword);
		m_Lexer.AddLexeme(":", TokenT::Keyword);
		m_Lexer.AddLexeme("(", TokenT::Keyword);
		m_Lexer.AddLexeme(")", TokenT::Keyword);
		m_Lexer.AddLexeme("{", TokenT::Keyword);
		m_Lexer.AddLexeme("}", TokenT::Keyword);
		m_Lexer.AddLexeme(",", TokenT::Keyword);
		m_Lexer.AddLexeme("->", TokenT::Keyword);

		AddOperator("::", OperDesc(0, AssocT::Right));
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

	void Parser::AddOperator(std::string sym, OperDesc desc)
	{
		m_Lexer.AddLexeme(sym, TokenT::Operator);
		m_Operators.insert(std::make_pair(sym, desc));
	}

	OperDesc* Parser::GetOperator(Token t)
	{
		if (t.Type == TokenT::Operator)
		{
			auto it = m_Operators.find(t.Value);
			if (it == m_Operators.end())
			{
				std::cout << "OPERATOR ASSERTION" << std::endl;
				return nullptr;
			}
			return &it->second;
		}

		return nullptr;
	}

	std::vector<Stmt*> Parser::ParseProgram(const char* src)
	{
		m_Lexer.SetSource(src);
		Next();

		std::vector<Stmt*> prog;
		//Stmt* st = nullptr;
		//while (st = ParseConstDecl()) prog.push_back(st);

		Expr* exp = ParseExpr();
		std::cout << exp->ToString() << std::endl;
		for (;;);

		return prog;
	}

	Stmt* Parser::ParseStmt()
	{
		Stmt* st = nullptr;
		//if (st = ParseConstDecl()) return st;

		return nullptr;
	}

	Expr* Parser::ParseExpr()
	{
		return ParsePrecedence();
	}

	static void debug_stack(std::vector<StackElem> const& st)
	{
		std::cout << "--------STACK TRACE----------" << std::endl;
		for (auto& t : st)
		{
			if (t.Tag == StackElemT::Expr)
			{
				std::cout << t.Exp->ToString() << std::endl;
			}
			else
			{
				std::cout << "OP: '" << t.Oper.Value << "'" << std::endl;
			}

			std::cout << "-----------------------------" << std::endl;
		}
		std::cout << "=============================" << std::endl;
	}

	Expr* Parser::ParsePrecedence()
	{
		std::vector<StackElem> stack;

		OperDesc* op = nullptr;
		Expr* exp = nullptr;
		while (true)
		{
			if (op = GetOperator(m_CurrentToken))
			{
				Token t = m_CurrentToken;
				Next();
				stack.push_back(StackElem(t));
			}
			else if (exp = ParseAtom())
			{
				stack.push_back(StackElem(exp));
			}
			else
			{
				break;
			}
		}

		while (true)
		{
			auto prevsz = stack.size();
			Reduce(0, stack.size(), stack);
			if (stack.size() == prevsz)
			{
				if (stack.size() == 1 && stack[0].Tag == StackElemT::Expr)
				{
					return stack[0].Exp;
				}
				else
				{
					std::cout << "EXPR ERROR" << std::endl;
					return nullptr;
				}
			}
		}

		return nullptr;
	}

	double Parser::GetHighestPrecedence(std::size_t from, std::size_t to, std::vector<StackElem>& stack)
	{
		double max = -1;

		OperDesc* op = nullptr;
		for (std::size_t i = from; i < to; i++)
		{
			auto& el = stack[i];

			if (el.Tag == StackElemT::Oper && (op = GetOperator(el.Oper)))
			{
				if (op->Precedence > max)
				{
					max = op->Precedence;
				}
			}
		}

		return max;
	}

	std::vector<std::size_t> Parser::GetAllPrecedenceIndex(std::size_t from, std::size_t to, double p, std::vector<StackElem>& stack)
	{
		std::vector<std::size_t> idxes;
		OperDesc* op = nullptr;
		for (std::size_t i = from; i < to; i++)
		{
			StackElem& e = stack[i];

			if (e.Tag == StackElemT::Oper && (op = GetOperator(e.Oper)))
			{
				if (op->Precedence == p)
				{
					idxes.push_back(i);
				}
			}
		}

		return idxes;
	}

	void Parser::ReduceInfixAt(std::size_t idx, std::vector<StackElem>& stack)
	{
		if (stack[idx - 1].Tag == StackElemT::Expr &&
			stack[idx + 1].Tag == StackElemT::Expr)
		{
			Expr* lhs = stack[idx - 1].Exp;
			Expr* rhs = stack[idx + 1].Exp;
			Expr* nexp = new BinExpr(lhs, rhs, stack[idx].Oper.Value);

			// Delete LHS, operator and RHS
			stack.erase(stack.begin() + (idx - 1));
			stack.erase(stack.begin() + (idx - 1));
			stack.erase(stack.begin() + (idx - 1));
			// Insert new element
			if (idx - 1 == stack.size())
				stack.push_back(StackElem(nexp));
			else
				stack.insert(stack.begin() + (idx - 1), StackElem(nexp));
		}
		else
		{
			std::cout << "Not operands around infix op!" << std::endl;
			return;
		}
	}

	void Parser::Reduce(std::size_t from, std::size_t to, std::vector<StackElem>& stack)
	{
		double maxprec = GetHighestPrecedence(from, to, stack);
		if (maxprec >= 0)
		{
			auto list = GetAllPrecedenceIndex(from, to, maxprec, stack);

			// Reduce INFIX LEFT
			for (std::size_t i = 0; i < list.size(); i++)
			{
				std::size_t idx = list[i];
				OperDesc* op = GetOperator(stack[idx].Oper);
				if (op->Fixity == FixityT::Infix && op->Assoc == AssocT::Left)
				{
					if (idx == 0 || idx == stack.size() - 1)
					{
						std::cout << "Infix at end or beginning!" << std::endl;
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
				OperDesc* op = GetOperator(stack[idx].Oper);
				if (op->Fixity == FixityT::Infix && op->Assoc == AssocT::Right)
				{
					if (idx == 0 || idx == stack.size() - 1)
					{
						std::cout << "Infix at end or beginning!" << std::endl;
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
				OperDesc* op = GetOperator(stack[idx].Oper);
				if (op->Fixity == FixityT::Postfix)
				{
					if (idx == 0)
					{
						std::cout << "Postfix at beginning!" << std::endl;
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
						std::cout << "No operands for postfix operator!" << std::endl;
						return;
					}

					OperDesc* op2 = nullptr;
					for (j = j - 1; j >= 0; j -= 2)
					{
						if ((j > 0) &&
							(stack[j].Tag == StackElemT::Oper &&
								stack[j - 1].Tag == StackElemT::Expr))
						{
							op2 = GetOperator(stack[j].Oper);
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
						Expr* lhs = stack[idx - 2].Exp;
						Expr* rhs = stack[idx - 1].Exp;
						Expr* nexp = new BinExpr(lhs, rhs, stack[idx].Oper.Value);

						// Delete LHS, RHS and operator
						stack.erase(stack.begin() + (idx - 2));
						stack.erase(stack.begin() + (idx - 2));
						stack.erase(stack.begin() + (idx - 2));

						// Insert new
						if (idx - 2 == stack.size())
							stack.push_back(StackElem(nexp));
						else
							stack.insert(stack.begin() + (idx - 2), StackElem(nexp));
					}
					return;
				}
			}

			// Reduce PREFIX
			for (std::size_t i = 0; i < list.size(); i++)
			{
				std::size_t idx = list[i];
				OperDesc* op = GetOperator(stack[idx].Oper);
				if (op->Fixity == FixityT::Prefix)
				{
					if (idx == stack.size() - 1)
					{
						std::cout << "Prefix at end!" << std::endl;
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
						std::cout << "No operands for prefix operator!" << std::endl;
						return;
					}

					OperDesc* op2 = nullptr;
					for (j = j + 1; j < stack.size(); j += 2)
					{
						if ((j < stack.size() - 1) &&
							(stack[j].Tag == StackElemT::Oper &&
								stack[j + 1].Tag == StackElemT::Expr))
						{
							op2 = GetOperator(stack[j].Oper);
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
						Expr* lhs = stack[idx + 1].Exp;
						Expr* rhs = stack[idx + 2].Exp;
						Expr* nexp = new BinExpr(lhs, rhs, stack[idx].Oper.Value);

						// Delete LHS, RHS and operator
						stack.erase(stack.begin() + (idx));
						stack.erase(stack.begin() + (idx));
						stack.erase(stack.begin() + (idx));

						// Insert new
						if (idx == stack.size())
							stack.push_back(StackElem(nexp));
						else
							stack.insert(stack.begin() + idx, StackElem(nexp));
					}
					return;
				}
			}
		}
	}

	Expr* Parser::ParseAtom()
	{
		if (m_CurrentToken.Type == TokenT::Integer)
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
				std::cout << "')' expected!" << std::endl;
				return nullptr;
			}
		}

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