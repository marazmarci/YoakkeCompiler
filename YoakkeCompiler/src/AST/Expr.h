#pragma once

#include <string>
#include <vector>
#include "Node.h"
#include "../Types.h"
#include "../Parsing/Token.h"

namespace yk
{
	// Pre-declare types that are used as pointers
	class Stmt;
	class TypeSymbol;
	class TypedSymbol;
	class TypeDesc;
	class Operator;

	// Base class for any expression
	class Expr : public Node
	{
	public:
		TypeSymbol* EvalType;
		TypeSymbol* HintType;

	protected:
		Expr(NodePos const& p);

	public:
		virtual ~Expr();
	};
	
	// Integer literal
	class IntLiteralExpr : public Expr
	{
	public:
		yint Value;

	public:
		IntLiteralExpr(Token const& tok);
		virtual ~IntLiteralExpr();

	public:
		virtual XMLNode* ToXML() override;
	};

	// An identifier with an optional hint type for ambigous evaluation
	class IdentExpr : public Expr
	{
	public:
		ystr Ident;
		TypedSymbol* Sym;

	public:
		IdentExpr(Token const& tok);
		virtual ~IdentExpr();

	public:
		virtual XMLNode* ToXML() override;
	};

	// Anything between curly braces is a block expression
	class BlockExpr : public Expr
	{
	public:
		yvec<Stmt*> Statements;

	public:
		BlockExpr(yvec<Stmt*> const& st, NodePos const& p);
		virtual ~BlockExpr();

	public:
		virtual XMLNode* ToXML() override;
	};

	// Parameter for function
	class ParamExpr : public Expr
	{
	public:
		ystr Value;
		TypeDesc* Type;

	public:
		ParamExpr(Token* v, TypeDesc* t);
		virtual ~ParamExpr();

	public:
		virtual XMLNode* ToXML() override;
	};
	
	// A function header declaration
	class FuncHeaderExpr : public Expr
	{
	public:
		yvec<ParamExpr*> Parameters;
		TypeDesc* ReturnType;

	public:
		FuncHeaderExpr(yvec<ParamExpr*> const& pars, TypeDesc* ret, NodePos const& p);
		virtual ~FuncHeaderExpr();

	public:
		virtual XMLNode* ToXML() override;
	};

	// A function with implementation
	class FuncExpr : public Expr
	{
	public:
		FuncHeaderExpr* Prototype;
		BlockExpr* Body;

	public:
		FuncExpr(FuncHeaderExpr* proto, BlockExpr* body);
		virtual ~FuncExpr();

	public:
		virtual XMLNode* ToXML() override;
	};

	class OperExpr : public Expr
	{
	public:
		Operator* OP;

	public:
		OperExpr(Operator* o, Token const& t);
		virtual ~OperExpr();

	public:
		virtual XMLNode* ToXML() override;
	};

	// An expression chained with an operator
	class UryExpr : public Expr
	{
	public:
		Expr* Sub;
		OperExpr* OP;

	public:
		UryExpr(Expr* s, OperExpr* o);
		virtual ~UryExpr();

	public:
		virtual XMLNode* ToXML() override;
	};

	// Two expressions connected with an operator
	class BinExpr : public Expr
	{
	public:
		Expr* LHS;
		Expr* RHS;
		OperExpr* OP;

	public:
		BinExpr(Expr* l, Expr* r, OperExpr* o);
		virtual ~BinExpr();

	public:
		virtual XMLNode* ToXML() override;
	};

	// An expression enclosed between 2 tokens (needed for parsing)
	class EnclosedExpr : public Expr
	{
	public:
		Token Begin;
		Token End;
		Expr* Sub;

	public:
		EnclosedExpr(Expr* s, Token const& l, Token const& r);
		virtual ~EnclosedExpr();

	public:
		virtual XMLNode* ToXML() override;
	};

	// Variable declaration (+ definition)
	class LetExpr : public Expr
	{
	public:
		Expr* Lvalue;
		TypeDesc* Type;
		Expr* Value;

	public:
		LetExpr(Expr* lv, TypeDesc* td, Expr* val, NodePos const& p);
		virtual ~LetExpr();

	public:
		virtual XMLNode* ToXML() override;
	};
}