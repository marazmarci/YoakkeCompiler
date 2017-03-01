#pragma once

#include "../parse/position.h"
#include "../utility/visitor.h"

// TODO: abstract away common structures into templates (binop, list, ident)

namespace yk {
	class ast_node {
	public:
		const position Position;

	public:
		ast_node(position const& pos);
		virtual ~ast_node();
	};
}