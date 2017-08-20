#pragma once

#include "../common.h"
#include "../syntax/position.h"

enum class symbol_t {
	Constant,
	Variable,
	Typeclass,
};

struct type;

struct symbol {
public:
	symbol_t		Ty;
	ystr			Name;
	yopt<interval>	DefPos;

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

struct typeclass_symbol : public symbol {
	yvec<type*>	Types;

	typeclass_symbol(ystr const& name, type* typ);
	virtual ~typeclass_symbol();
};

