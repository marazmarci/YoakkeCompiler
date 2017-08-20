#pragma once

#include "../common.h"
#include "../syntax/position.h"

namespace type_prefixes {
	constexpr auto Tuple = "@tup";
	constexpr auto Function = "@fn";
	constexpr auto Params = "@params";
}

enum class type_t {
	Variable,
	Constructor,
	Set,
};

struct type {
public:
	type_t Ty;
	yopt<interval> DefPos;

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

	type_cons(ystr const& name, yvec<type*> const& params = {});
	virtual ~type_cons();

	void add(type* t);

	static type_cons* fn(type* params, type* ret);
	static type_cons* generic_fn();
};

struct type_set : public type {
	yvec<type*>& Types;

	type_set(yvec<type*>& ts);
	virtual ~type_set();
};
