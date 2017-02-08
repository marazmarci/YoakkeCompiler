#pragma once

#include "Node.h"

namespace yk
{
	namespace ast
	{
		class Expr;

		class Stmt : public Node
		{
		protected:
			Stmt(parse::Position const& pos);

		public:
			virtual ~Stmt();
		};

		class ExprStmt : public Stmt
		{
		public:
			Expr* Expression;
			bool Semicol;

		public:
			ExprStmt(Expr* exp);
			ExprStmt(Expr* exp, parse::Token const& sc);
			virtual ~ExprStmt();
		};
	}
}