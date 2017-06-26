/**
 * ast_expr.h
 * Created by: Peter Lenkefi
 * All rights reserved.
 *
 * Description:
 *  Here are all the expression-related nodes of the AST.
 */

#pragma once

#include <cstdlib>
#include "ast.h"
#include "token.h"

struct AST_ty;
struct AST_stmt;

/**
 * This enum contains all the types an expression node can have.
 * Used for faster casting.
 */
enum class AST_expr_t {
	Ident,
	IntLit,
	BinOp,
	PreOp,
	PostOp,
	Func,
	Call,
	If,
};

/**
 * This is the base-class for all expression AST nodes.
 */
struct AST_expr : public AST_node {
public:
	AST_expr_t Type;	// Type of node

protected:
	/**
	 * Creates an expression node with a given position and type.
	 * @param pos The position of the represented node in the file.
	 * @param ty The expresstion type.
	 */
	AST_expr(interval const& pos, AST_expr_t ty);

public:
	virtual ~AST_expr();

public:
	/**
	 * Check if this node is a braced construct.
	 * @return True, if braced.
	 */
	bool is_braced() const;
};

/**
 * An identifier expression is a single word for identifying variables,
 * functions and so on.
 */
struct AST_ident_expr : public AST_expr {
	ystr ID;	// The name

	/**
	 * Creates an identifier expression from a given token.
	 * @param tok The token that represents the identifier.
	 */
	AST_ident_expr(token const& tok);

	virtual ~AST_ident_expr();
};

/**
 * An integer literal expression represents an integer literal number.
 */
struct AST_int_lit_expr : public AST_expr {
	long long int Value;	// Integer value

	/**
	 * Creates an integer literal expression from a given token.
	 * @param tok The token that represents the number.
	 */
	AST_int_lit_expr(token const& tok);

	virtual ~AST_int_lit_expr();
};

/**
 * A prefix unary expression represents an operation on a single node.
 */
struct AST_pre_expr : public AST_expr {
	token			OP;		// Operator
	AST_expr*		Sub;	// Operand

	/**
	 * Creates a prefix unary expression with the given operand and operator.
	 * @param op The operator token.
	 * @param sub The operand.
	 */
	AST_pre_expr(token const& op, AST_expr* sub);

	virtual ~AST_pre_expr();
};

/**
 * A postfix unary expression represents an operation on a single node.
 * Similar to prefix, but the operator is after the operand.
 */
struct AST_post_expr : public AST_expr {
	token			OP;		// Operator
	AST_expr*		Sub;	// Operand

	/**
	 * Creates a postfix unary expression with the given operand and operator.
	 * @param op The operator token.
	 * @param sub The operand.
	 */
	AST_post_expr(token const& op, AST_expr* sub);

	virtual ~AST_post_expr();
};

/**
 * A binary expression represents an operation between two expression nodes.
 */
struct AST_bin_expr : public AST_expr {
	token			OP;		// Operator between nodes
	AST_expr*		LHS;	// Left-hand side operand
	AST_expr*		RHS;	// Right-hand side operand

	/**
	 * Creates a binary expression with the given operands and operator.
	 * @param op The operator token.
	 * @param left The left-hand side operand.
	 * @param right The right-hand side operand.
	 */
	AST_bin_expr(token const& op, AST_expr* left, AST_expr* right);

	virtual ~AST_bin_expr();
};

/**
 * This represents a braced code block without a name, used for other language
 * constructs like functions and if statements.
 * Does not inherit AST_expr, as this is just a helper construct.
 */
struct AST_body_expr {
	// TODO: capture block
	interval		Pos;		// Position in the file
	yvec<AST_stmt*>	Statements;	// The statements of the body
	AST_expr*		Return;		// The return expression at the end

	/**
	 * Creates a braced expression from statements.
	 * @param beg The beginning brace.
	 * @param end The ending brace.
	 * @param stmts The list of statements.
	 * @param ret The return expression at the end (may be null).
	 */
	AST_body_expr(token const& beg, token const& end,
		yvec<AST_stmt*> const& stmts, AST_expr* ret = nullptr);

	/**
	 * Creates a braced expression from a single statement.
	 * @param stmt The statement to create the body from.
	 */
	AST_body_expr(AST_stmt* stmt);

	~AST_body_expr();
};

// Helper for the function expression, a type representing a param
// An optional identifier and a type
using param_t = ypair<yopt<token>, AST_ty*>;

/**
 * A function expression is a lambda declaration with an optional name.
 * In that case it's just a normal function.
 */
struct AST_func_expr : public AST_expr {
	yopt<token>		Label;	// An optional name
	yvec<param_t>	Args;	// The list of arguments
	AST_ty*			Return;	// The return type
	AST_body_expr*	Body;	// The body containing the code

	/**
	 * Creates a function expression with the given arguments, return type
	 * and body.
	 * @param beg The beginning 'fn' token.
	 * @param args The list of arguments.
	 * @param ret The declared return type (may be null).
	 * @param body The body of the function.
	 * @param name An optional name, making it a normal named function.
	 */
	AST_func_expr(
		token const& beg,
		yopt<token> name,
		yvec<param_t> const& args, AST_ty* ret,
		AST_body_expr* body);

	virtual ~AST_func_expr();
};

/**
 * A call expression calls a function expression with a given list of 
 * arguments.
 */
struct AST_call_expr : public AST_expr {
	AST_expr*		Func;	// The function expression
	yvec<AST_expr*>	Args;	// The arguments to call with

	/**
	 * Creates a function call expression for a given function with the given
	 * parameters.
	 * @param fn The function expression.
	 * @param args The list of arguments.
	 * @param end The right parenthese of the call.
	 */
	AST_call_expr(AST_expr* fn, yvec<AST_expr*> const& args, token const& end);

	virtual ~AST_call_expr();
};

/**
 * An if expression runs the body based on the evaluation of the condition.
 */
struct AST_if_expr : public AST_expr {
	yopt<token>		Label;		// An optional name
	AST_expr*		Condition;	// When to run the Then block
	AST_ty*			Return;		// Return type (may be null)
	AST_body_expr*	Then;		// Run if condition is true
	AST_body_expr*	Else;		// Run if false (may be null)

	/**
	 * Creates an if expression.
	 * @param beg The 'if' token.
	 * @param name An optional label name.
	 * @param cond The condition.
	 * @param ret An optional return type.
	 * @param then The body followed by the condition and return type.
	 * @param els An optional else body.
	 */
	AST_if_expr(
		token const& beg,
		yopt<token> name,
		AST_expr* cond, AST_ty* ret,
		AST_body_expr* then, AST_body_expr* els);

	virtual ~AST_if_expr();
};
