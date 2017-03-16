#pragma once

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
