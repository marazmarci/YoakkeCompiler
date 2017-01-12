#include <iostream>
#include "src\Parser.h"

std::map<std::string, int> vals;

int eval(yk::Expr* exp)
{
	if (dynamic_cast<yk::BinExpr*>(exp))
	{
		yk::BinExpr* be = (yk::BinExpr*)exp;
		std::string& op = be->OP->Symbol;
		if (op[op.length() - 1] == '+') return eval(be->LHS) + eval(be->RHS);
		if (op[op.length() - 1] == '-') return eval(be->LHS) - eval(be->RHS);
		if (op[op.length() - 1] == '*') return eval(be->LHS) * eval(be->RHS);
		if (op[op.length() - 1] == '/') return eval(be->LHS) / eval(be->RHS);

		if (op == ":=")
		{
			if (dynamic_cast<yk::VarExpr*>(be->LHS))
			{
				int val = eval(be->RHS);
				vals.insert(std::make_pair(((yk::VarExpr*)(be->LHS))->Identifier, val));
				return val;
			}
			else
			{
				std::cout << "LHS of := must be a variable!" << std::endl;
				return 0;
			}
		}
	}
	else if (dynamic_cast<yk::VarExpr*>(exp))
	{
		yk::VarExpr* ve = (yk::VarExpr*)exp;
		auto it = vals.find(ve->Identifier);
		if (it == vals.end())
		{
			std::cout << "Unknown variable: " << ve->Identifier << std::endl;
			return 0;
		}
		return it->second;
	}
	else
	{
		return ((yk::IntConstExpr*)exp)->Value;
	}
}

int main(void)
{
	for (;;)
	{
		std::string line;
		std::getline(std::cin, line);
		yk::Parser parser;
		auto prog = parser.Parse(line);
		for (auto n : prog)
		{
			if (dynamic_cast<yk::Expr*>(n))
			{
				std::cout << (yk::Expr*)n << std::endl;
				std::cout << "RESULT: " << eval((yk::Expr*)n) << std::endl;
			}
		}
	}

	return 0;
}