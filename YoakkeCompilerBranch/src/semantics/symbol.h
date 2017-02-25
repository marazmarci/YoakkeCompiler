#pragma once

#include "../common.h"

namespace yk {
	class symbol {
	public:
		ystr Identifier;

	public:
		symbol(ystr const& ident);
		virtual ~symbol();
	};
}