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
	type*			Type;
	ystr			Name;
	yopt<interval>	DefPos;

protected:
	symbol(symbol_t t, type* ty, ystr const& name);

public:
	virtual ~symbol();
};

struct const_symbol : public symbol {
	const_symbol(ystr const& name, type* typ);
	virtual ~const_symbol();
};

struct var_symbol : public symbol {
	var_symbol(ystr const& name, type* typ);
	virtual ~var_symbol();
};

struct typeclass_symbol : public symbol {
	yvec<type*>	Types;

	typeclass_symbol(ystr const& name);
	virtual ~typeclass_symbol();

	void add(type* ty);
};

