#pragma once

#include "../common.h"

struct type;
struct type_var;

namespace unifier {
	ystr to_str(type* ty);
	type* prune(type* ty);
	bool contains(type* container, type_var* what);
	bool same(type* aa, type* bb);
	bool matches_fn_fn(type* a, type* b);
	bool matches(type* a, type* b);
	yopt<ystr> unify(type* a, type* b);
}
