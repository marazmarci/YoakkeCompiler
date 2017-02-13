#pragma once

#include "Node.h"
#include "../Parsing/Token.h"

namespace yk
{
	namespace ast
	{
		/*
		TODO:
		 - Move UnitTestPrint() out of the tree?
		*/

		class TypeDesc;

		// Abstract expression
		class Expr : public Node
		{
		protected:
			Expr(parse::Position const& pos);

		public:
			virtual ~Expr();

		public:
			virtual ystr UnitTestPrint() = 0;
		};

		// An identifier referring to a symbol
		class IdentExpr : public Expr
		{
		public:
			ystr Identifier;

		public:
			IdentExpr(parse::Token const& tok);
			virtual ~IdentExpr();

		public:
			virtual ystr UnitTestPrint() override;
		};

		// Abstract unary expression
		class UryExpr : public Expr
		{
		public:
			parse::Token Operator;
			Expr* Sub;

		protected:
			UryExpr(parse::Token const& op, Expr* s, parse::Position const& pos);

		public:
			virtual ~UryExpr();
		};

		// Prefix unary expression
		class PrefixExpr : public UryExpr
		{
		public:
			PrefixExpr(Expr* sub, parse::Token const& op);
			virtual ~PrefixExpr();

		public:
			virtual ystr UnitTestPrint() override;
		};

		// Postfix unary expression
		class PostfixExpr : public UryExpr
		{
		public:
			PostfixExpr(Expr* sub, parse::Token const& op);
			virtual ~PostfixExpr();

		public:
			virtual ystr UnitTestPrint() override;
		};

		// Binary expression
		class BinExpr : public Expr
		{
		public:
			parse::Token Operator;
			Expr* LHS;
			Expr* RHS;

		public:
			BinExpr(Expr* l, Expr* r, parse::Token const& op);
			virtual ~BinExpr();

		public:
			virtual ystr UnitTestPrint() override;
		};

		// Mixfix expression
		class MixfixExpr : public Expr
		{
		public:
			ystr Symbol;
			yvec<Expr*> Operands;

		public:
			MixfixExpr(ystr const& sym, yvec<Expr*> const& opers, parse::Position const& pos);
			~MixfixExpr();

		public:
			virtual ystr UnitTestPrint() override;
		};

		// Enclosed expression
		class EnclosedExpr : public Expr
		{
		public:
			parse::Token Left;
			parse::Token Right;
			Expr* Sub;

		public:
			EnclosedExpr(parse::Token const& l, parse::Token const& r, Expr* s);
			~EnclosedExpr();

		public:
			virtual ystr UnitTestPrint() override;
		};

		// Let expression
		class LetExpr : public Expr
		{
		public:
			Expr* Lvalue;
			Expr* Rvalue;
			TypeDesc* Type;

		public:
			LetExpr(Expr* l, Expr* r, TypeDesc* t);
			virtual ~LetExpr();

		public:
			virtual ystr UnitTestPrint() override;
		};
	}
}