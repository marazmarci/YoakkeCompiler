/**
 * ast_pat.h
 * Created by: Peter Lenkefi
 * All rights reserved.
 *
 * Description:
 *  These are the patterns represented in the AST.
 */

#pragma once

#include "ast.h"
#include "token.h"

/**
 * This enum contains all the types a pattern node can have.
 * Used for faster casting.
 */
enum class AST_pat_t {
	Ident,
	List,
};

/**
 * This is the base-class for all pattern AST nodes.
 */
struct AST_pat : public AST_node {
public:
	AST_pat_t Type;		// Type of node

protected:
	/**
	 * Creates a pattern node with a given position and type.
	 * @param pos The position of the represented node in the file.
	 * @param ty The type descriptor type.
	 */
	AST_pat(interval const& pos, AST_pat_t ty);

public:
	virtual ~AST_pat();
};

/**
 * An identifier pattern is a single word representing a 1 to 1 connection
 * in a pattern.
 */
struct AST_ident_pat : public AST_pat {
	ystr ID;	// The name

	/**
	 * Creates an identifier pattern from a given token.
	 * @param tok The token that represents the identifier.
	 */
	AST_ident_pat(token const& tok);

	virtual ~AST_ident_pat();
};

/**
 * A list pattern is used to separate elements in a tuple.
 */
struct AST_list_pat : public AST_pat {
	yvec<AST_pat*> Elements;	// The contained elements

	/**
	 * Creates a list pattern from a vector of patterns.
	 * @param elems The vector of elements.
	 */
	AST_list_pat(yvec<AST_pat*> const& elems);

	/**
	 * Creates an empty list pattern just from a pair of parenthesis.
	 * @param beg The opening parenthese token.
	 * @param end The closing parenthese token.
	 */
	AST_list_pat(token const& beg, token const& end);

	virtual ~AST_list_pat();
};
