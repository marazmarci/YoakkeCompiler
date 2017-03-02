#pragma once

#include "../parse/position.h"
#include "../utility/visitor.h"

/*
The base class for the nodes in the AST.
Contains positional information.
*/

namespace yk {
	class ast_node {
	public:
		const position Position;

	public:
		ast_node(position const& pos);
		virtual ~ast_node();
	};
}