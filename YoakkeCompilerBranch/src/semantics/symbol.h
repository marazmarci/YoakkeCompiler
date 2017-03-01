#pragma once

#include "../common.h"

namespace yk {
	class symbol {
	public:
		ystr identifier;

	public:
		symbol(ystr const& ident);
		virtual ~symbol();
	};
}