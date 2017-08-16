#pragma once

#include "../common.h"

struct type;
struct symbol;

struct scope {
	scope*				Parent;
	ymap<ystr, type*>	Types;
	ymap<ystr, symbol*>	Symbols;

	scope();
	~scope();

	yopt<type*> ref_type(ystr const& name);
	yopt<symbol*> ref_sym(ystr const& name);

	yopt<type*> local_ref_type(ystr const& name);
	yopt<symbol*> local_ref_sym(ystr const& name);

	void decl_type(type* ty);
	void decl_sym(symbol* sym);

	// TODO: shadow(sym, ty): name -> 'name -> ''name
};
