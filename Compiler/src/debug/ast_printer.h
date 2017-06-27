/**
 * ast_printer.h
 * Created by: Peter Lenkefi
 * All rights reserved.
 *
 * Description:
 *  The AST printer outputs a visual representation of the AST for debugging
 *  purposes.
 *  The current format is very primitive, it is like the Windows tree command.
 *
 * TODO:
 *  There should be a better format, something like:
 *       +
 *    ___|___
 *   /       \
 *  2         *
 *          __|__
 *         /     \
 *        3       4
 */

#pragma once

#include <iostream>
#include "../syntax/ast_expr.h"

struct AST_printer {
public:
	static std::ostream* Out;	// The output stream

public:
	/**
	 * Prints a given node and all it's subnodes.
	 * @param exp The root node to print.
	 * @param indent The current depth of the tree, 0 by default.
	 */
	static void print(const AST_expr* exp, ysize indent = 0);

	/**
	 * Prints a given node and all it's subnodes.
	 * @param stmt The root node to print.
	 * @param indent The current depth of the tree, 0 by default.
	 */
	static void print(const AST_stmt* stmt, ysize indent = 0);

private:
	static void print_indent(ysize indent);
};
