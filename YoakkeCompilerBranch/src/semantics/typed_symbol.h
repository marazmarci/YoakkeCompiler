#pragma once

#include "symbol.h"

namespace yk {
	class type_symbol;
	class expr;

	class typed_symbol : public symbol {
	public:
		type_symbol* Type;

	protected:
		typed_symbol(ystr const& id, type_symbol* ts);

	public:
		virtual ~typed_symbol();
	};

	class const_bind_symbol : public typed_symbol {
	public:
		expr* Value;

	public:
		const_bind_symbol(ystr const& id, type_symbol* ts, expr* exp);
		virtual ~const_bind_symbol();
	};

	class var_symbol : public typed_symbol {
	public:
		var_symbol(ystr const& id, type_symbol* ts);
		virtual ~var_symbol();
	};
}