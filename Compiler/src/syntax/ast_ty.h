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
	AST_ty(interval const& pos, AST_ty_t ty)
		: AST_node(pos), Type(ty) {
	}

public:
	virtual ~AST_ty() { }
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
	AST_ident_ty(token const& tok)
		: AST_ty(tok.Pos, AST_ty_t::Ident), ID(tok.Value) {
	}

	virtual ~AST_ident_ty() { }
};
