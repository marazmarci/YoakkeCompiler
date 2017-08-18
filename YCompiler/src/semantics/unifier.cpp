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
		}
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
		}
	}

	bool contains(type* container, type_var* what) {
		type* ty = prune(container);

		if (ty->Ty == type_t::Variable) {
			type_var* tv = (type_var*)ty;
			return tv->ID == what->ID;
		}
		if (ty->Ty == type_t::Constructor) {
			type_cons* tc = (type_cons*)ty;
			for (type* p : tc->Params) {
				if (contains(p, what)) {
					return true;
				}
			}
			return false;
		}

		UNREACHABLE;
	}

	bool same(type* a, type* b) {
		type* t1 = prune(a);
		type* t2 = prune(b);

		if (   t1->Ty == type_t::Variable
			&& t2->Ty == type_t::Variable) {
			type_var* tt1 = (type_var*)t1;
			type_var* tt2 = (type_var*)t2;

			return tt1->ID == tt2->ID;
		}
		else if (t1->Ty == type_t::Constructor
			  && t2->Ty == type_t::Constructor) {
			type_cons* tt1 = (type_cons*)t1;
			type_cons* tt2 = (type_cons*)t2;

			if (tt1->Name != tt2->Name) {
				return false;
			}
			if (tt1->Params.size() != tt2->Params.size()) {
				return false;
			}
			for (ysize i = 0; i < tt1->Params.size(); i++) {
				if (!same(tt1->Params[i], tt2->Params[i])) {
					return false;
				}
			}
			return true;
		}
		else {
			return false;
		}

		UNREACHABLE;
	}

	yopt<ystr> unify(type* a, type* b) {
		type* t1 = prune(a);
		type* t2 = prune(b);

		if (t1->Ty == type_t::Variable) {
			type_var* tt1 = (type_var*)t1;
			if (t2->Ty == type_t::Variable) {
				type_var* tt2 = (type_var*)t2;
				if (tt1->ID != tt2->ID) {
					tt1->Instance = tt2;
				}
				return {};
			}
			else if (t2->Ty == type_t::Constructor) {
				if (contains(t2, tt1)) {
					return ystr("Recursive type: " + to_str(t2) + " contains " + to_str(t1));
				}
				tt1->Instance = t2;
				return {};
			}
			else {
				assert(false && "Unreachable!");
			}
		}
		else if (t1->Ty == type_t::Constructor) {
			type_cons* tt1 = (type_cons*)t1;
			if (t2->Ty == type_t::Variable) {
				type_var* tt2 = (type_var*)t2;
				if (contains(t1, tt2)) {
					return ystr("Recursive type: " + to_str(t1) + " contains " + to_str(t2));
				}
				tt2->Instance = t1;
				return {};
			}
			else if (t2->Ty == type_t::Constructor) {
				type_cons* tt2 = (type_cons*)t2;
				if (tt1->Name != tt2->Name) {
					return ystr(to_str(t1) + " is not " + to_str(t2));
				}
				if (tt1->Params.size() != tt2->Params.size()) {
					return ystr(to_str(t1) + " has different number of type parameters than " + to_str(t2));
				}
				for (ysize i = 0; i < tt1->Params.size(); i++) {
					if (auto err = unify(tt1->Params[i], tt2->Params[i])) {
						return *err;
					}
				}
				return {};
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
