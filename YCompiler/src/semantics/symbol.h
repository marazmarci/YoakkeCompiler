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
	type* Type;

	const_symbol(ystr const& name, type* typ);
	virtual ~const_symbol();
};

struct var_symbol : public symbol {
	type* Type;

	var_symbol(ystr const& name, type* typ);
	virtual ~var_symbol();
};

struct typeclass_symbol : public symbol {
	yvec<const_symbol*> Instances;

	typeclass_symbol(ystr const& name, const_symbol* ins1);
	virtual ~typeclass_symbol();

	yopt<const_symbol*> add(const_symbol* t);
};
