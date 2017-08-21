#include <cassert>
#include "type.h"
#include "unifier.h"

namespace unifier {
	ystr to_str(type* ty) {
		switch (ty->Ty) {
		case type_t::Variable: {
			type_var* tv = (type_var*)ty;
			if (tv->Instance) {
				return to_str(tv->Instance);
			}
			return ystr{ '\'', char('A' + tv->ID % 26) };
		}

		case type_t::Constructor: {
			type_cons* tc = (type_cons*)ty;
			if (tc->Name == type_prefixes::Function) {
				assert(tc->Params.size() == 2);
				return to_str(tc->Params[0]) + " -> " + to_str(tc->Params[1]);
			}
			else if (tc->Name == type_prefixes::Tuple) {
				ystr res = "(";
				if (tc->Params.size()) {
					res += to_str(tc->Params[0]);
					for (ysize i = 1; i < tc->Params.size(); i++) {
						res += ", " + to_str(tc->Params[i]);
					}
				}
				res += ')';
				return res;
			}
			else {
				ystr res = tc->Name;
				if (tc->Params.size()) {
					res += '<' + to_str(tc->Params[0]);
					for (ysize i = 1; i < tc->Params.size(); i++) {
						res += ", " + to_str(tc->Params[i]);
					}
					res += '>';
				}
				return res;
			}

			UNREACHABLE;
		}

		default: UNIMPLEMENTED;
		}

		UNREACHABLE;
	}

	type* prune(type* ty) {
		switch (ty->Ty) {
		case type_t::Variable: {
			type_var* tv = (type_var*)ty;
			if (tv->Instance) {
				tv->Instance = prune(tv->Instance);
				return tv->Instance;
			}
			return tv;
		}

		case type_t::Constructor: {
			return ty;
		}

		default: UNIMPLEMENTED;
		}

		UNREACHABLE;
	}

	bool contains(type* container, type_var* what) {
		type* ty = prune(container);

		switch (ty->Ty) {
		case type_t::Variable: {
			type_var* tv = (type_var*)ty;
			return tv->ID == what->ID;
		}

		case type_t::Constructor: {
			type_cons* tc = (type_cons*)ty;
			for (type* p : tc->Params) {
				if (contains(p, what)) {
					return true;
				}
			}
			return false;
		}

		default: UNIMPLEMENTED;
		}

		UNREACHABLE;
	}

	bool same(type* a, type* b) {
		type* t1 = prune(a);
		type* t2 = prune(b);

		type_t tt1 = t1->Ty;
		type_t tt2 = t1->Ty;

		if (tt1 == tt2) {
			switch (tt1) {
			case type_t::Constructor: {
				type_var* ta = (type_var*)t1;
				type_var* tb = (type_var*)t2;

				return ta->ID == tb->ID;
			}

			case type_t::Variable: {
				type_cons* ta = (type_cons*)t1;
				type_cons* tb = (type_cons*)t2;

				if (ta->Name != tb->Name) {
					return false;
				}
				if (ta->Params.size() != tb->Params.size()) {
					return false;
				}
				for (ysize i = 0; i < ta->Params.size(); i++) {
					if (!same(ta->Params[i], tb->Params[i])) {
						return false;
					}
				}
				return true;
			}

			default: UNIMPLEMENTED;
			}
		}

		return false;
	}

	namespace {
		yopt<ystr> unify_var_var(type_var* tv1, type_var* tv2) {
			if (tv1->ID != tv2->ID) {
				tv1->Instance = tv2;
			}
			return {};
		}

		yopt<ystr> unify_var_cons(type_var* tt1, type_cons* tt2) {
			if (contains(tt2, tt1)) {
				return ystr("Recursive type: " + to_str(tt2) + " contains " + to_str(tt1));
			}
			tt1->Instance = tt2;
			return {};
		}

		yopt<ystr> unify_cons_cons(type_cons* tt1, type_cons* tt2) {
			if (tt1->Name != tt2->Name) {
				return ystr(to_str(tt1) + " is not " + to_str(tt2));
			}
			if (tt1->Params.size() != tt2->Params.size()) {
				return ystr(to_str(tt1) + " has different number of type parameters than " + to_str(tt2));
			}
			for (ysize i = 0; i < tt1->Params.size(); i++) {
				if (auto err = unify(tt1->Params[i], tt2->Params[i])) {
					return *err;
				}
			}
			return {};
		}
	}

	yopt<ystr> unify(type* a, type* b) {
		type* t1 = prune(a);
		type* t2 = prune(b);

		if (t1->Ty == type_t::Variable) {
			type_var* tt1 = (type_var*)t1;

			if (t2->Ty == type_t::Variable) {
				type_var* tt2 = (type_var*)t2;
				return unify_var_var(tt1, tt2);
			}
			else if (t2->Ty == type_t::Constructor) {
				type_cons* tt2 = (type_cons*)t2;
				return unify_var_cons(tt1, tt2);
			}
			else {
				UNREACHABLE;
			}
		}
		else if (t1->Ty == type_t::Constructor) {
			type_cons* tt1 = (type_cons*)t1;

			if (t2->Ty == type_t::Variable) {
				type_var* tt2 = (type_var*)t2;
				return unify_var_cons(tt2, tt1);
			}
			else if (t2->Ty == type_t::Constructor) {
				type_cons* tt2 = (type_cons*)t2;
				return unify_cons_cons(tt1, tt2);
			}
			else {
				UNREACHABLE;
			}
		}
		else {
			UNREACHABLE;
		}

		UNREACHABLE;
	}
}
