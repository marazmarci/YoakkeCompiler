#pragma once

#include "semantic_pos.h"
#include "../common.h"

enum class symbol_t {
	Constant,
	Variable,
	Typeclass,
};

struct type;
struct type_cons;

struct symbol {
public:
	symbol_t			Ty;
	ystr				Name;
	yopt<semantic_pos>	DefPos;

protected:
	symbol(symbol_t t, ystr const& name);

public:
	virtual ~symbol();
};

struct const_symbol : public symbol {
	type*	Type;

	const_symbol(ystr const& name, type* typ);
	virtual ~const_symbol();
};

struct var_symbol : public symbol {
	type*	Type;

	var_symbol(ystr const& name, type* typ);
	virtual ~var_symbol();
};

struct typeclass_symbol : public symbol {
	yvec<type_cons*> Instances;
	type* Type;

	typeclass_symbol(ystr const& name, type_cons* typ1, type_cons* typ2);
	virtual ~typeclass_symbol();

	void add(type_cons* t);
};
