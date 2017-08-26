#pragma once

#include "class_constraint.h"
#include "../common.h"

struct type;
struct type_var;
struct type_cons;
struct typeclass_symbol;

namespace unifier {
	ystr to_str(type* ty);
	ystr to_str(class_constraint const& cc);
	type* prune(type* ty);
	bool contains(type* container, type_var* what);
	bool same(type* aa, type* bb);
	bool matches_fn_fn(type* a, type* b);
	bool matches_fn_fn_c(type_cons* t1, type_cons* t2);
	bool matches(type* a, type* b);
	yopt<ystr> unify(type* a, type* b);
	type_cons* add_class_constraint(yvec<class_constraint>& ls, typeclass_symbol* tc);
	bool process_class_constraint_list(yvec<class_constraint>& ls);
	bool process_class_constraint(class_constraint& cc);
}
