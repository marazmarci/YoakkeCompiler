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
	AST_expr(interval const& pos, AST_expr_t ty)
		: AST_node(pos), Type(ty) {
	}

public:
	virtual ~AST_expr() { }
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
	AST_ident_expr(token const& tok)
		: AST_expr(tok.Pos, AST_expr_t::Ident), ID(tok.Value) {
	}

	virtual ~AST_ident_expr() { }
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
	AST_int_lit_expr(token const& tok)
		: AST_expr(tok.Pos, AST_expr_t::IntLit),
		Value(std::atoll(tok.Value.c_str())) {
	}

	virtual ~AST_int_lit_expr() { }
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
	AST_pre_expr(token const& op, AST_expr* sub)
		: AST_expr(interval(op.Pos, sub->Pos), AST_expr_t::PreOp),
		OP(op), Sub(sub) {
	}

	virtual ~AST_pre_expr() {
		delete Sub;
	}
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
	AST_post_expr(token const& op, AST_expr* sub)
		: AST_expr(interval(sub->Pos, op.Pos), AST_expr_t::PostOp),
		OP(op), Sub(sub) {
	}

	virtual ~AST_post_expr() {
		delete Sub;
	}
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
	AST_bin_expr(token const& op, AST_expr* left, AST_expr* right)
		: AST_expr(interval(left->Pos, right->Pos), AST_expr_t::BinOp),
		OP(op), LHS(left), RHS(right) {
	}

	virtual ~AST_bin_expr() {
		delete LHS;
		delete RHS;
	}
};

/**
 * This represents a braced code block without a name, used for other language
 * constructs like functions and if statements.
 */
struct AST_body_expr : public AST_expr {
	// TODO: capture block
};

// Helper for the function expression, a type representing a param
// An optional identifier and a type
using param_t = ypair<yopt<token>, AST_ty*>;

/**
 * A function expression is a lambda declaration with an optional name.
 * In that case it's just a normal function.
 */
struct AST_func_expr : public AST_expr {
	yvec<param_t>	Args;	// The list of arguments
	AST_ty*			Return;	// The return type
	AST_body_expr*	Body;	// The body containing the code
	yopt<token>		Label;	// An optional name

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
		yvec<param_t> const& args, AST_ty* ret, 
		AST_body_expr* body,
		yopt<token> name = {})
		: AST_expr(interval(beg.Pos, body->Pos), AST_expr_t::Func),
		Args(args), Return(ret), Body(body), Label(name) {
	}
};
