#pragma once

struct scope;
struct type_const;

struct symbol_table {
	scope* Current;
	scope* Global;

	symbol_table();
	~symbol_table();

	void push_scope();
	void pop_scope();

	void decl(type_cons* tc);
	void decl(symbol* sym);

	yopt<symbol*> ref_sym(ystr const& name);
	yopt<type*> ref_type(ystr const& name);

	yopt<symbol*> local_ref_sym(ystr const& name);
	yopt<type*> local_ref_type(ystr const& name);

	yopt<symbol*> upper_ref_sym(ystr const& name);
	yopt<type*> upper_ref_type(ystr const& name);
};
