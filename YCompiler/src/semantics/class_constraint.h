#pragma once

struct type_cons;
struct typeclass_symbol;

struct class_constraint {
	typeclass_symbol*	Typeclass;
	type_cons*			ToMatch;

	class_constraint(typeclass_symbol* tc, type_cons* m)
		: Typeclass(tc), ToMatch(m) {
	}
};
