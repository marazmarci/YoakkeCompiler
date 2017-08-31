#pragma once

#include "semantic_pos.h"

struct type_cons;
struct typeclass_symbol;

struct class_constraint {
	typeclass_symbol*	Typeclass;
	type_cons*			ToMatch;
	semantic_pos		Position;

	class_constraint(typeclass_symbol* tc, type_cons* m, semantic_pos const& pos)
		: Typeclass(tc), ToMatch(m), Position(pos) {
	}
};
