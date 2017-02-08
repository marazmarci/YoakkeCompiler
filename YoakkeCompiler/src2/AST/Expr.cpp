#include "Expr.h"

namespace yk
{
	namespace ast
	{
		// Abstract expression
		Expr::Expr(parse::Position const& pos)
			: Node(pos)
		{
		}

		Expr::~Expr()
		{
		}

		// Ident expression
		IdentExpr::IdentExpr(parse::Token const& tok)
			: Expr(parse::Position::Get(tok)), Identifier(tok.Value)
		{
		}

		IdentExpr::~IdentExpr()
		{
		}

		ystr IdentExpr::UnitTestPrint()
		{
			return Identifier;
		}

		// Abstract unary expression
		UryExpr::UryExpr(parse::Token const& op, Expr* s, parse::Position const& pos)
			: Expr(pos), Operator(op), Sub(s)
		{
		}

		UryExpr::~UryExpr()
		{
			delete Sub;
		}

		// Prefix unary expression
		PrefixExpr::PrefixExpr(Expr* sub, parse::Token const& op)
			: UryExpr(op, sub, 
				parse::Position::Interval(parse::Position::Get(op), sub->Pos))
		{
		}

		PrefixExpr::~PrefixExpr()
		{
		}

		ystr PrefixExpr::UnitTestPrint()
		{
			return '(' + Operator.Value + Sub->UnitTestPrint() + ')';
		}

		// Postfix unary expression
		PostfixExpr::PostfixExpr(Expr* sub, parse::Token const& op)
			: UryExpr(op, sub,
				parse::Position::Interval(sub->Pos, parse::Position::Get(op)))
		{
		}

		PostfixExpr::~PostfixExpr()
		{
		}

		ystr PostfixExpr::UnitTestPrint()
		{
			return '(' + Sub->UnitTestPrint() + Operator.Value + ')';
		}

		// Binary expression
		BinExpr::BinExpr(Expr* l, Expr* r, parse::Token const& op)
			: Expr(parse::Position::Interval(l->Pos, r->Pos)), 
			LHS(l), RHS(r), Operator(op)
		{
		}

		BinExpr::~BinExpr()
		{
			delete LHS;
			delete RHS;
		}

		ystr BinExpr::UnitTestPrint()
		{
			return '(' + LHS->UnitTestPrint() + ' ' + Operator.Value + ' '
				+ RHS->UnitTestPrint() + ')';
		}
	}
}