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

/**
 * This enum contains all the types an expression node can have.
 * Used for faster casting.
 */
enum class expr_t {
	Ident,
	IntLit,
	BinOp,
	PreOp,
	PostOp,
};

/**
 * This is the base-class for all expression AST nodes.
 */
struct AST_expr : public AST_node {
public:
	expr_t Type;	// Type of node

protected:
	/**
	 * Creates an expression node with a given position and type.
	 * @param pos The position of the represented node in the file.
	 * @param ty The expresstion type.
	 */
	AST_expr(interval const& pos, expr_t ty)
		: AST_node(pos), Type(ty) {
	}
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
		: AST_expr(tok.Pos, expr_t::Ident), ID(tok.Value) {
	}
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
		: AST_expr(tok.Pos, expr_t::IntLit), 
		Value(std::atoll(tok.Value.c_str())) {
	}
};

/**
 * A prefix unary expression represents an operation on a single node.
 */
struct AST_pre_expr : public AST_expr {
	token			OP;		// Operator
	ysptr<AST_expr> Sub;	// Operand

	/**
	 * Creates a prefix unary expression with the given operand and operator.
	 * @param op The operator token.
	 * @param sub The operand.
	 */
	AST_pre_expr(token const& op, ysptr<AST_expr> sub)
		: AST_expr(interval(op.Pos, sub->Pos), expr_t::PreOp),
		OP(op), Sub(sub) {
	}
};

/**
 * A postfix unary expression represents an operation on a single node.
 * Similar to prefix, but the operator is after the operand.
 */
struct AST_post_expr : public AST_expr {
	token			OP;		// Operator
	ysptr<AST_expr> Sub;	// Operand

	/**
	 * Creates a postfix unary expression with the given operand and operator.
	 * @param op The operator token.
	 * @param sub The operand.
	 */
	AST_post_expr(token const& op, ysptr<AST_expr> sub)
		: AST_expr(interval(sub->Pos, op.Pos), expr_t::PostOp),
		OP(op), Sub(sub) {
	}
};

/**
 * A binary expression represents an operation between two expression nodes.
 */
struct AST_bin_expr : public AST_expr {
	token			OP;		// Operator between nodes
	ysptr<AST_expr> LHS;	// Left-hand side operand
	ysptr<AST_expr> RHS;	// Right-hand side operand

	/**
	 * Creates a binary expression with the given operands and operator.
	 * @param op The operator token.
	 * @param left The left-hand side operand.
	 * @param right The right-hand side operand.
	 */
	AST_bin_expr(token const& op, ysptr<AST_expr> left, ysptr<AST_expr> right)
		: AST_expr(interval(LHS->Pos, RHS->Pos), expr_t::BinOp),
		OP(op), LHS(left), RHS(right) {
	}
};
