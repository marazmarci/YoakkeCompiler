#pragma once

#include "symbol.h"

namespace yk {
	class type_symbol : public symbol {
	protected:
		type_symbol(ystr const& id);

	public:
		virtual ~type_symbol();
	};

	class native_type_symbol : public type_symbol {
	public:
		native_type_symbol(ystr const& id);
		virtual ~native_type_symbol();
	};
}
