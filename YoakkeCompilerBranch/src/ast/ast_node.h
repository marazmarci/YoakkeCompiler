#pragma once

#include "../parse/position.h"
#include "../utility/double_dispatcher.h"

namespace yk {
	class ast_node : public dispatchable {
	public:
		const position Position;

	public:
		ast_node(position const& pos);
		virtual ~ast_node();
	};
}