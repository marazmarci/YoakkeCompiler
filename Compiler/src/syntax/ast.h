/**
 * ast.h
 * Created by: Peter Lenkefi
 * All rights reserved.
 *
 * Description:
 *  Here is the AST node base-class that is inherited by every AST node.
 */

#pragma once

#include "position.h"

struct AST_node {
public:
	interval Pos;	// Position in the file

protected:
	/**
	 * Creates a new node with the given position.
	 * @param pos The position of the represented node in the file.
	 */
	AST_node(interval const& pos)
		: Pos(pos) {
	}

public:
	virtual ~AST_node() { }
};
