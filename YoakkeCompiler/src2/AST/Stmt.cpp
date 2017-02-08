#include "Stmt.h"
#include "Expr.h"

namespace yk
{
	namespace ast
	{
		Stmt::Stmt(parse::Position const& pos)
			: Node(pos)
		{
		}

		Stmt::~Stmt()
		{
		}

		ExprStmt::ExprStmt(Expr* exp)
			: Stmt(exp->Pos), Semicol(false)
		{
		}
		
		ExprStmt::ExprStmt(Expr* exp, parse::Token const& sc)
			: Stmt(parse::Position::Interval(exp->Pos, parse::Position::Get(sc))), 
			Semicol(true)
		{
		}

		ExprStmt::~ExprStmt()
		{
		}
	}
}