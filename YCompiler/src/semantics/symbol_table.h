#pragma once

struct scope;

struct symbol_table {
	scope* Current;
	scope* Global;

	symbol_table();
	~symbol_table();

	void push_scope();
	void pop_scope();
};
