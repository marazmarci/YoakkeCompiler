#pragma once

#include "../common.h"

namespace yk {
	class symbol {
	public:
		const ystr Identifier;

	protected:
		symbol(ystr const& id);

	public:
		virtual ~symbol();
	};
}
