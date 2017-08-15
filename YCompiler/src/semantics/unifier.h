#pragma once

#include "../common.h"

struct type;
struct type_var;

namespace unifier {
	ystr to_str(type* ty);
	type* prune(type* ty);
	bool contains(type* container, type_var* what);
	yopt<ystr> unify(type* a, type* b);
}
