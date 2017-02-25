#pragma once

#include "symbol.h"

namespace yk {
	class type_symbol : public symbol {
	protected:
		type_symbol(ystr const& id);

	public:
		virtual ~type_symbol();
	};

	class builtin_type_symbol : public type_symbol {
	public:
		builtin_type_symbol(ystr const& id);
		virtual ~builtin_type_symbol();
	};
}