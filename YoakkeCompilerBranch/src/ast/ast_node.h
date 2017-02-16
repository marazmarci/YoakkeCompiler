#pragma once

#include "../parse/position.h"

namespace yk {
	class ast_node {
	public:
		position Position;

	public:
		ast_node(position const& pos);
		virtual ~ast_node();
	};
}