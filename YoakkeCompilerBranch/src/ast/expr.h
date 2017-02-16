#pragma once

#include "ast_node.h"

namespace yk {
	class expr : public ast_node {
	public:
		expr(position const& pos);
		virtual ~expr();
	};
}