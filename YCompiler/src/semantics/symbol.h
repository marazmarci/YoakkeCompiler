#pragma once

#include "../common.h"

enum class symbol_t {
	Constant,
	Variable,
	OverloadSet,
};

struct type;

struct symbol {
public:
	symbol_t	Ty;
	ystr		Name;

protected:
	symbol(symbol_t t, ystr const& name);

public:
	virtual ~symbol();
};

struct const_symbol : public symbol {
	type*		Type;

	const_symbol(ystr const& name, type* typ);
	virtual ~const_symbol();
};

struct var_symbol : public symbol {
	type*		Type;

	var_symbol(ystr const& name, type* typ);
	virtual ~var_symbol();
};

struct overload_set_symbol : public symbol {
	yvec<type*>	Types;

	overload_set_symbol(ystr const& name, type* typ);
	virtual ~overload_set_symbol();
};

