#pragma once

#include "symbol.h"

namespace yk {
	class type_symbol : public symbol {
	protected:
		type_symbol(ystr const& id);

	public:
		virtual ~type_symbol();

	public:
		virtual bool match(type_symbol* other) = 0;
	};

	class builtin_type_symbol : public type_symbol {
	public:
		builtin_type_symbol(ystr const& id);
		virtual ~builtin_type_symbol();

	public:
		virtual bool match(type_symbol* other) override;
	};

	class tuple_type_symbol : public type_symbol {
	public:
		yvec<type_symbol*> Types;

	public:
		tuple_type_symbol(yvec<type_symbol*> const& ts);
		virtual ~tuple_type_symbol();

	public:
		virtual bool match(type_symbol* other) override;
	};
}