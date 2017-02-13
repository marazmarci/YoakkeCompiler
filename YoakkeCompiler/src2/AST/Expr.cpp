#include "Expr.h"
#include "TypeDesc.h"

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

		// Mixfix expression
		MixfixExpr::MixfixExpr(ystr const& sym, yvec<Expr*> const& opers, parse::Position const& pos)
			: Expr(pos), Symbol(sym), Operands(opers)
		{
		}

		MixfixExpr::~MixfixExpr()
		{
			for (auto o : Operands)
				delete o;
		}

		ystr MixfixExpr::UnitTestPrint()
		{
			ystr ret = Symbol + '(';
			if (Operands.size())
			{
				ret += Operands[0]->UnitTestPrint();
				for (ysize i = 1; i < Operands.size(); i++)
					ret += ", " + Operands[i]->UnitTestPrint();
			}
			ret += ')';
			return ret;
		}

		// Enclosed expression
		EnclosedExpr::EnclosedExpr(parse::Token const& l, parse::Token const& r, Expr* s)
			: Expr(parse::Position::Interval(parse::Position::Get(l),
				parse::Position::Get(r))), Left(l), Right(r), Sub(s)
		{
		}
		
		EnclosedExpr::~EnclosedExpr()
		{
			delete Sub;
		}

		ystr EnclosedExpr::UnitTestPrint()
		{
			return Sub->UnitTestPrint();
		}

		// Let expression
		LetExpr::LetExpr(Expr* l, Expr* r, TypeDesc* t)
			: Expr(parse::Position::Interval(l->Pos,
				r ? r->Pos : (t ? t->Pos : l->Pos))), Lvalue(l), Rvalue(r),Type(t)
		{
		}
		
		LetExpr::~LetExpr()
		{
			delete Lvalue;
			if (Rvalue) delete Rvalue;
			if (Type) delete Type;
		}
		
		ystr LetExpr::UnitTestPrint()
		{
			ystr ret = "let (" + Lvalue->UnitTestPrint() + ')';
			if (Rvalue) ret += " = (" + Rvalue->UnitTestPrint() + ')';
			return ret;
		}
	}
}