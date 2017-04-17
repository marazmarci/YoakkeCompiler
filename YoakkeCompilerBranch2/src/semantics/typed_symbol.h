#pragma once

#include "../common.h"
#include "symbol.h"

namespace yk {
	class type_symbol;

	class typed_symbol : public symbol {
	public:
		yshared_ptr<type_symbol> Type;

	protected:
		typed_symbol(ystr const& id, yshared_ptr<type_symbol> ty);

	public:
		virtual ~typed_symbol();
	};

	class const_typed_symbol : public typed_symbol {
	public:
		const_typed_symbol(ystr const& id, yshared_ptr<type_symbol> ty);
		virtual ~const_typed_symbol();
	};

	class var_typed_symbol : public typed_symbol {
	public:
		var_typed_symbol(ystr const& id, yshared_ptr<type_symbol> ty);
		virtual ~var_typed_symbol();
	};
}
