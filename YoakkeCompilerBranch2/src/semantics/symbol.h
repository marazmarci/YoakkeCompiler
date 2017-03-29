#pragma once

#include "../common.h"

namespace yk {
	class symbol {
	public:
		ystr Identifier;

	protected:
		symbol(ystr const& id);
	};
}
