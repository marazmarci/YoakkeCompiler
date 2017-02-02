#pragma once

#include "../Parsing/Token.h"
#include <string>
#include <vector>
#include "Node.h"
#include "../Parsing/Operator.h"

namespace yk
{
	// Pre-declare types that are used as pointers
	class Stmt;
	class TypeSymbol;
	class TypedSymbol;
	class TypeDesc;

	// Base class for any expression
	// Children must override ToString, which is an XML dump of the subtree
	class Expr : public Node
	{
	public:
		TypeSymbol* EvalType;

	protected:
		Expr(NodePos const& p);

	public:
		virtual ~Expr();
	};

	// Any literal holding the constant value
	template <typename T>
	class LiteralExpr : public Expr
	{
	public:
		T Value;

	public:
		LiteralExpr(T const& v, NodePos const& p)
			: Expr(p), Value(v)
		{
		}

	public:
		virtual std::string ToXML() override
		{
			std::string name = std::string(typeid(T).name()) + "Literal";
			return "<" + name + ">" + std::to_string(Value) + "</" + name + ">";
		}
	};
	typedef LiteralExpr<int> IntLiteralExpr;

	// An identifier with an optional hint type for ambigous evaluation
	class IdentExpr : public Expr
	{
	public:
		std::string Ident;
		TypeSymbol* HintType;
		TypedSymbol* Sym;

	public:
		IdentExpr(Token const& tok);
		virtual ~IdentExpr();

	public:
		virtual std::string ToXML() override;
	};

	// Anything between curly braces is a block expression
	class BlockExpr : public Expr
	{
	public:
		std::vector<Stmt*> Statements;

	public:
		BlockExpr(std::vector<Stmt*> const& st, NodePos const& p);
		virtual ~BlockExpr();

	public:
		virtual std::string ToXML() override;
	};

	// Parameter for function
	struct Parameter
	{
	public:
		Token Value;
		TypeDesc* Type;

	public:
		Parameter(Token const& v, TypeDesc* t);
		virtual ~Parameter();
	};
	
	// A function header declaration
	class FuncHeaderExpr : public Expr
	{
	public:
		std::vector<Parameter> Parameters;
		TypeDesc* ReturnType;

	public:
		FuncHeaderExpr(std::vector<Parameter> const& pars, TypeDesc* ret, NodePos const& p);
		virtual ~FuncHeaderExpr();

	public:
		virtual std::string ToXML() override;
	};

	// A function with implementation
	class FuncExpr : public Expr
	{
	public:
		FuncHeaderExpr* Prototype;
		BlockExpr* Body;

	public:
		FuncExpr(FuncHeaderExpr* proto, BlockExpr* body, NodePos const& p);
		virtual ~FuncExpr();

	public:
		virtual std::string ToXML() override;
	};

	// An expression chained with an operator
	class UryExpr : public Expr
	{
	public:
		Expr* Sub;
		OperPos OP;

	public:
		UryExpr(Expr* s, OperPos const& o);
		virtual ~UryExpr();

	public:
		virtual std::string ToXML() override;
	};

	// Two expressions connected with an operator
	class BinExpr : public Expr
	{
	public:
		Expr* LHS;
		Expr* RHS;
		OperPos OP;

	public:
		BinExpr(Expr* l, Expr* r, OperPos const& o);
		virtual ~BinExpr();

	public:
		virtual std::string ToXML() override;
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
		virtual std::string ToXML() override;
	};
}