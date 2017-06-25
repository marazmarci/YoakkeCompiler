/**
 * ast_stmt.h
 * Created by: Peter Lenkefi
 * All rights reserved.
 *
 * Description:
 *  These are all the statements and declarations in the AST.
 */

#pragma once

#include "ast.h"
#include "ast_expr.h"

/**
 * This enum contains all the types a statement node can have.
 * Used for faster casting.
 */
enum class AST_stmt_t {
	Expr,
};

/**
 * This is the base-class for all statement AST nodes.
 */
struct AST_stmt : public AST_node {
public:
	AST_stmt_t Type;	// Type of node

protected:
	/**
	* Creates a statement node with a given position and type.
	* @param pos The position of the represented node in the file.
	* @param ty The statement type.
	*/
	AST_stmt(interval const& pos, AST_stmt_t ty)
		: AST_node(pos), Type(ty) {
	}

public:
	virtual ~AST_stmt() { }
};

/**
 * This is a wrapper for expressions which act as statements.
 * They are either braced constructs used outside of expressions
 * or expressions ending with a semicolon.
 */
struct AST_expr_stmt : public AST_stmt {
	AST_expr* Sub;	// The wrapped expression

	/**
	 * Creates an expression statement from an expression.
	 * @param sub The expression to wrap.
	 * @param semicol Optional semicolon token.
	 */
	AST_expr_stmt(AST_expr* sub, yopt<token> semicol = {})
		: AST_stmt(semicol ? interval(sub->Pos, semicol->Pos) : sub->Pos
		, AST_stmt_t::Expr),
		Sub(sub) {
	}

	virtual ~AST_expr_stmt() {
		delete Sub;
	}
};

