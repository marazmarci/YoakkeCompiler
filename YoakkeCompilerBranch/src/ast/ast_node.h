#pragma once

#include "../parse/position.h"
#include "../utility/visitor.h"

/*
The base class for the nodes in the AST.
Contains positional information for error reporting.
*/

namespace yk {
	class ast_node {
	public:
		const position Position;

	protected:
		ast_node(position const& pos);

	public:
		virtual ~ast_node();
	};
}