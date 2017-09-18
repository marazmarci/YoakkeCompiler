#pragma once

#include "semantic_pos.h"
#include "../ir/ir_opcode.h"
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

enum class const_symbol_t {
	Builtin,
	Function,
	Value,
};

struct const_symbol : public symbol {
public:
	const_symbol_t ConstTy;
	type* Type;

protected:
	const_symbol(const_symbol_t ct, ystr const& name, type* typ);

public:
	virtual ~const_symbol();
};

struct builtin_const_symbol : public const_symbol {
	ir_opcode Opcode;

	builtin_const_symbol(ystr const& name, type* typ, ir_opcode opc);
	virtual ~builtin_const_symbol();
};

struct fn_const_symbol : public const_symbol {
	fn_const_symbol(ystr const& name, type* typ);
	virtual ~fn_const_symbol();
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
