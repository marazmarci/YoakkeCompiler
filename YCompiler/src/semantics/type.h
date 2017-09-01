#pragma once

#include "semantic_pos.h"
#include "../common.h"
#include "../syntax/position.h"

namespace type_prefixes {
	constexpr auto Tuple	= "@tup";
	constexpr auto Function = "@fn";
}

enum class type_t {
	Variable,
	Constructor,
};

struct type {
public:
	type_t Ty;
	yopt<semantic_pos>	DefPos;

protected:
	type(type_t tt);

public:
	virtual ~type();
};

struct type_var : public type {
	ysize				ID;
	type*				Instance;

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

	template <typename... Ts>
	static type_cons* tuple(Ts... tys) {
		return new type_cons(type_prefixes::Tuple, { tys... });
	}
};
