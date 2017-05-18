#pragma once

/*
The node class is the base-class for all of the AST nodes.
Contains positional data.
*/

#include "../common.h"
#include "../lexing/position.h"

namespace yk {
	class node {
	public:
		interval Position;

	protected:
		node(interval const& pos);

	public:
		virtual ~node();
	};
}
