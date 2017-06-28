/**
 * ast_ty.h
 * Created by: Peter Lenkefi
 * All rights reserved.
 *
 * Description:
 *  These are all the type descriptors of the AST.
 */

#pragma once

#include "ast.h"
#include "token.h"

/**
 * This enum contains all the types a type descriptor node can have.
 * Used for faster casting.
 */
enum class AST_ty_t {
	Ident,
	BinOp,
	List,
};

/**
 * This is the base-class for all type descriptor AST nodes.
 */
struct AST_ty : public AST_node {
public:
	AST_ty_t Type;	// Type of node

protected:
	/**
	 * Creates a type descriptor node with a given position and type.
	 * @param pos The position of the represented node in the file.
	 * @param ty The type descriptor type.
	 */
	AST_ty(interval const& pos, AST_ty_t ty);

public:
	virtual ~AST_ty();
};

/**
 * An identifier type descriptor is a single word for build-in types or
 * type aliases.
 */
struct AST_ident_ty : public AST_ty {
	ystr ID;	// The name

	/**
	 * Creates an identifier type descriptor from a given token.
	 * @param tok The token that represents the identifier.
	 */
	AST_ident_ty(token const& tok);

	virtual ~AST_ident_ty();
};

/**
 * A binary type descriptor combines two types constructing a new type.
 * Used for example for functions: A -> B.
 */
struct AST_bin_ty : public AST_ty {
	token		OP;		// Operator between nodes
	AST_ty*		LHS;	// Left-hand side operand
	AST_ty*		RHS;	// Right-hand side operand

	/**
	 * Creates a binary type descriptor with the given operands and operator.
	 * @param op The operator token.
	 * @param left The left-hand side operand.
	 * @param right The right-hand side operand.
	 */
	AST_bin_ty(token const& op, AST_ty* left, AST_ty* right);

	virtual ~AST_bin_ty();
};

/**
 * A list type is a composition of types separated by a comma creating a tuple.
 */
struct AST_list_ty : public AST_ty {
	yvec<AST_ty*> Elements;	// The contained elements

	/**
	 * Creates a tuple type from a vector of types.
	 * @param elems The vector of elements.
	 */
	AST_list_ty(yvec<AST_ty*> const& elems);

	/**
	 * Creates an empty list type just from a pair of parenthesis.
	 * @param beg The opening parenthese token.
	 * @param end The closing parenthese token.
	 */
	AST_list_ty(token const& beg, token const& end);

	virtual ~AST_list_ty();
};
