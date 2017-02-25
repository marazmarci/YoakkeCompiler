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
		virtual bool same(type_symbol* other);
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

	class func_type_symbol : public type_symbol {
	public:
		yvec<type_symbol*> ArgTypes;
		type_symbol* ReturnType;

	public:
		func_type_symbol(yvec<type_symbol*> const& args, type_symbol* rett);
		virtual ~func_type_symbol();

	public:
		virtual bool match(type_symbol* other) override;
		virtual bool same(type_symbol* other) override;
	};

	class builtin_func_type_symbol : public func_type_symbol {
	public:
		builtin_func_type_symbol(yvec<type_symbol*> const& args, type_symbol* rett);
		virtual ~builtin_func_type_symbol();
	};
}