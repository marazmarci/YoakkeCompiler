#pragma once

#include "../common.h"

namespace yk {
	class expr;
	class type_symbol;

	class let_meta {
	public:
		yvec<ytriplet<ystr, expr*, type_symbol*>> Values;

	public:
		void add(ystr const& id, expr* val, type_symbol* ts);
	};
}
