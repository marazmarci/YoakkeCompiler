#pragma once

#include "symbol.h"
#include "type.h"

namespace yk {
	class variable : public symbol {
	public:
		ysptr<type> Type;

	public:
		variable(ysptr<type> t);
	};
}
