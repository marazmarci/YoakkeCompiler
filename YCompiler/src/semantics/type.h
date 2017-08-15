#pragma once

#include "../common.h"

enum class type_t {
	Variable,
	Constructor,
};

struct type {
public:
	type_t Ty;

protected:
	type(type_t tt);

public:
	virtual ~type();
};

struct type_var : public type {
	ysize ID;
	type* Instance;

	type_var();
	virtual ~type_var();

private:
	static ysize s_IDcounter;
};

struct type_cons : public type {
	ystr		Name;
	yvec<type*>	Params;

	type_cons(ystr const& name, yvec<type*> const& params);
	virtual ~type_cons();
};
